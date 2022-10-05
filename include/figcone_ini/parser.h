#ifndef FIGCONE_INI_PARSER_H
#define FIGCONE_INI_PARSER_H

#include "detail/paramparser.h"
#include "detail/external/sfun/string_utils.h"
#include "detail/external/inicpp.h"
#include <figcone_tree/tree.h>
#include <figcone_tree/iparser.h>
#include <figcone_tree/errors.h>
#include <regex>
#include <vector>
#include <string>
#include <map>
#include <sstream>

namespace figcone::ini::detail {

inline const auto fakeRootSectionId = std::string{"71cd27eb-7f27-4b61-a2ff-a64c89b275a7"};

inline bool isUnsignedInt(const std::string& str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

inline bool hasRootSection(std::istream& stream)
{
    auto pos = stream.tellg();
    auto firstWord = std::string{};
    stream >> firstWord;
    stream.seekg(pos);
    return firstWord.front() == '[';
}

inline std::string getStreamContent(std::istream& stream)
{
    stream >> std::noskipws;
    return {std::istream_iterator<char>{stream}, std::istream_iterator<char>{}};
}

inline void parseSection(const ini::IniSection& section, figcone::TreeNode& node)
{
    for (const auto& [key, value]: section) {
        auto paramList = readParamList(key, value.as<std::string>());
        if (paramList)
            node.asItem().addParamList(key, *paramList);
        else
            node.asItem().addParam(key, readParam(value.as<std::string>()));
    }
}

inline std::string afterLast(const std::string& str, const std::string& delim)
{
    auto pos = str.rfind(delim);
    if (pos == std::string::npos)
        return str;
    return str.substr(pos + delim.size());
}

inline std::string beforeLast(const std::string& str, const std::string& delim)
{
    auto pos = str.rfind(delim);
    if (pos == std::string::npos)
        return str;
    return str.substr(0, pos);
}

inline figcone::ConfigError makeConfigError(const std::exception& e, bool hasFakeRootSection)
{
    auto message = std::string{e.what()};
    auto regex = std::regex{R"(l\.(\d+): (.*))"};
    auto match = std::smatch{};
    if (std::regex_search(message, match, regex)){
        auto line = std::stoi(match[1]);
        if (hasFakeRootSection)
            line--;
        auto error = match[2].str();
        if (!error.empty())
            error[0] = static_cast<char>(std::toupper(error[0]));

        return {error, {line, {}}};
    }
    return {e.what()};
}
}

namespace figcone::ini {
class Parser : public IParser {
public:
    TreeNode parse(std::istream& stream) override
    {
        auto fixedInputStream = std::unique_ptr<std::istream>{};
        auto& input = [&]()->decltype(auto){
            if (detail::hasRootSection(stream))
                return stream;
            auto inputStr = "[" + detail::fakeRootSectionId + "]\n" + detail::getStreamContent(stream);
            fixedInputStream = std::make_unique<std::istringstream>(inputStr);
            return *fixedInputStream;
        }();

        auto ini = [&] {
            try {
                return ini::IniFile{input};
            }
            catch (const std::exception& e) {
                throw detail::makeConfigError(e, fixedInputStream != nullptr);
            }
        }();

        auto tree = figcone::makeTreeRoot();
        createSectionNodes(ini, tree);
        parseSections(ini, tree);
        return tree;
    }

private:
    void parseSections(const ini::IniFile& ini, figcone::TreeNode& node)
    {
        for (const auto& [sectionName, section]: ini)
            if (sectionName == detail::fakeRootSectionId)
                detail::parseSection(section, node);
            else if (!sectionArrays_.count(sectionName))
                detail::parseSection(section, *sections_.at(sectionName));
    }

    void createSectionNodes(const ini::IniFile& ini, figcone::TreeNode& node)
    {
        namespace str = sfun::string_utils;
        for (const auto& [sectionName, section]: ini){
            currentNode_ = &node;
            if (sectionName == detail::fakeRootSectionId)
                continue;
            auto newSectionName = std::string{};
            auto sectionParts = str::split(sectionName, ".");
            for (auto sectionPartIt = sectionParts.begin(); sectionPartIt != sectionParts.end(); ++sectionPartIt) {
                auto sectionPart = std::string{*sectionPartIt};
                newSectionName += sectionPart;
                if (std::next(sectionPartIt) != sectionParts.end() &&
                    detail::isUnsignedInt(std::string{*std::next(sectionPartIt)})) {
                    addSectionTreeNode(newSectionName, [&](auto& parentNode){
                        sectionArrays_[newSectionName] = 0;
                        return &parentNode.asItem().addNodeList(sectionPart);
                    });
                }
                else if (detail::isUnsignedInt(sectionPart)) {
                    addSectionTreeNode(newSectionName, [&](auto& parentNode){
                        auto listSectionName = detail::beforeLast(newSectionName, ".");
                        auto sectionArrayIndex = std::stoi(sectionPart);
                        if (sectionArrayIndex != sectionArrays_[listSectionName]) {
                            if (sectionArrayIndex != ++sectionArrays_[listSectionName])
                                throw figcone::ConfigError{"Section array index mismatch"};
                        }
                        return &parentNode.asList().addNode();
                    });
                }
                else {
                      addSectionTreeNode(newSectionName, [&](auto& parentNode){
                          return &parentNode.asItem().addNode(sectionPart);
                      });
                }
                newSectionName += ".";
            }
        }
    }

    void addSectionTreeNode(const std::string& sectionName, const std::function<figcone::TreeNode*(figcone::TreeNode&)>& nodeMaker)
    {
        if (!sections_.count(sectionName))
            sections_[sectionName] = nodeMaker(*currentNode_);
        currentNode_ = sections_[sectionName];
    }

    std::map<std::string, int> sectionArrays_;
    std::map<std::string, figcone::TreeNode*> sections_;
    figcone::TreeNode* currentNode_ = nullptr;
};

}

#endif //FIGCONE_INI_PARSER_H