#include "paramparser.h"
#include <inicpp.h>
#include <figcone_ini/parser.h>
#include <figcone_tree/errors.h>
#include <figcone_tree/tree.h>
#include <eel/string_utils.h>
#include <iterator>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace figcone::ini {

namespace {
const auto fakeRootSectionId = std::string{"71cd27eb-7f27-4b61-a2ff-a64c89b275a7"};

bool isUnsignedInt(const std::string& str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool hasRootSection(std::istream& stream)
{
    const auto pos = stream.tellg();
    auto firstWord = std::string{};
    stream >> firstWord;
    stream.seekg(pos);
    return firstWord.front() == '[';
}

std::string getStreamContent(std::istream& stream)
{
    stream >> std::noskipws;
    return {std::istream_iterator<char>{stream}, std::istream_iterator<char>{}};
}

void parseSection(const figcone::ini::IniSection& section, figcone::TreeNode& node)
{
    for (const auto& [key, value] : section) {
        if (const auto paramList = detail::readParamList(key, value.as<std::string>()))
            node.asItem().addParamList(key, *paramList);
        else
            node.asItem().addParam(key, detail::readParam(value.as<std::string>()));
    }
}

std::string beforeLast(const std::string& str, const std::string& delim)
{
    const auto pos = str.rfind(delim);
    if (pos == std::string::npos)
        return str;
    return str.substr(0, pos);
}

figcone::ConfigError makeConfigError(const std::exception& e, bool hasFakeRootSection)
{
    const auto message = std::string{e.what()};
    static const auto regex = std::regex{R"(l\.(\d+): (.*))"};
    auto match = std::smatch{};
    if (std::regex_search(message, match, regex)) {
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
} //namespace

class Parser::Impl {
public:
    void parseSections(const IniFile& ini, figcone::TreeNode& node);
    void createSectionNodes(const IniFile& ini, figcone::TreeNode& node);
    void addSectionTreeNode(
            const std::string& sectionName,
            const std::function<figcone::TreeNode*(figcone::TreeNode&)>& nodeMaker);

private:
    std::map<std::string, int> sectionArrays_;
    std::map<std::string, figcone::TreeNode*> sections_;
    figcone::TreeNode* currentNode_ = nullptr;
};

Parser::Parser()
    : impl_{std::make_unique<Parser::Impl>()}
{
}
Parser::~Parser() = default;

Tree Parser::parse(std::istream& stream)
{
    auto fixedInputStream = std::unique_ptr<std::istream>{};
    auto& input = [&]() -> decltype(auto)
    {
        if (hasRootSection(stream))
            return stream;
        auto inputStr = "[" + fakeRootSectionId + "]\n" + getStreamContent(stream);
        fixedInputStream = std::make_unique<std::istringstream>(inputStr);
        return *fixedInputStream;
    }();

    const auto ini = [&]
    {
        try {
            return IniFile{input};
        }
        catch (const std::exception& e) {
            throw makeConfigError(e, fixedInputStream != nullptr);
        }
    }();

    auto treeRoot = figcone::makeTreeRoot();
    impl_->createSectionNodes(ini, *treeRoot);
    impl_->parseSections(ini, *treeRoot);
    return Tree{std::move(treeRoot)};
}

void Parser::Impl::parseSections(const IniFile& ini, figcone::TreeNode& node)
{
    for (const auto& [sectionName, section] : ini)
        if (sectionName == fakeRootSectionId)
            parseSection(section, node);
        else if (!sectionArrays_.count(sectionName))
            parseSection(section, *sections_.at(sectionName));
}

void Parser::Impl::createSectionNodes(const IniFile& ini, figcone::TreeNode& node)
{
    for (const auto& [sectionName, section] : ini) {
        currentNode_ = &node;
        if (sectionName == fakeRootSectionId)
            continue;
        auto newSectionName = std::string{};
        auto sectionParts = eel::split(sectionName, ".");
        for (auto sectionPartIt = sectionParts.begin(); sectionPartIt != sectionParts.end(); ++sectionPartIt) {
            auto sectionPart = std::string{*sectionPartIt};
            newSectionName += sectionPart;
            if (std::next(sectionPartIt) != sectionParts.end() &&
                isUnsignedInt(std::string{*std::next(sectionPartIt)})) {
                addSectionTreeNode(
                        newSectionName,
                        [&](auto& parentNode)
                        {
                            sectionArrays_[newSectionName] = 0;
                            return &parentNode.asItem().addNodeList(sectionPart);
                        });
            }
            else if (isUnsignedInt(sectionPart)) {
                addSectionTreeNode(
                        newSectionName,
                        [&](auto& parentNode)
                        {
                            const auto listSectionName = beforeLast(newSectionName, ".");
                            const auto sectionArrayIndex = std::stoi(sectionPart);
                            if (sectionArrayIndex != sectionArrays_[listSectionName]) {
                                if (sectionArrayIndex != ++sectionArrays_[listSectionName])
                                    throw figcone::ConfigError{"Section array index mismatch"};
                            }
                            return &parentNode.asList().emplaceBack();
                        });
            }
            else {
                addSectionTreeNode(
                        newSectionName,
                        [&](auto& parentNode)
                        {
                            return &parentNode.asItem().addNode(sectionPart);
                        });
            }
            newSectionName += ".";
        }
    }
}

void Parser::Impl::addSectionTreeNode(
        const std::string& sectionName,
        const std::function<figcone::TreeNode*(figcone::TreeNode&)>& nodeMaker)
{
    if (!sections_.count(sectionName))
        sections_[sectionName] = nodeMaker(*currentNode_);
    currentNode_ = sections_[sectionName];
}

} //namespace figcone::ini
