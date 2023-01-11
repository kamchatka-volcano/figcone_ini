#include "paramparser.h"
#include "stream.h"
#include "utils.h"
#include <figcone_tree/errors.h>
#include <figcone_tree/tree.h>
#include <sstream>
#include <utility>

namespace figcone::ini::detail {

std::optional<std::string> readParam(
        Stream& stream,
        const std::string& wordSeparator,
        const std::vector<std::string>& paramListValue,
        const std::string& paramName)
{
    auto quotedParam = readQuotedString(stream);
    if (quotedParam)
        return *quotedParam;
    else {
        auto result = readWord(stream, wordSeparator);
        if (result.empty()) {
            if (stream.peek() == ",")
                throw ConfigError{"Parameter list '" + paramName + "' element is missing"};
            if (paramListValue.empty())
                return std::nullopt;
        }
        return result;
    }
}

std::optional<std::vector<std::string>> readParamList(const std::string& paramName, const std::string& paramValue)
{
    auto inputStream = std::stringstream{paramValue};
    auto stream = Stream{inputStream};
    skipWhitespace(stream, false);
    if (stream.peek() != "[")
        return std::nullopt;

    stream.skip(1);
    skipWhitespace(stream);
    auto paramValueList = std::vector<std::string>{};
    while (!stream.atEnd()) {
        auto param = readParam(stream, ",]", paramValueList, paramName);
        if (param)
            paramValueList.emplace_back(std::move(*param));

        skipWhitespace(stream, true);
        const auto endOfList = "]";
        if (stream.peek() == ",") {
            stream.skip(1);
            skipWhitespace(stream, true);
            if (stream.peek() == endOfList || stream.atEnd())
                throw ConfigError{"Parameter list '" + paramName + "' element is missing"};
        }
        else if (stream.peek() == endOfList) {
            stream.skip(1);
            return paramValueList;
        }
    }
    throw ConfigError{"Wrong parameter list '" + paramName + "' format: missing ']' at the end"};
}

std::string readParam(const std::string& paramValue)
{
    auto inputStream = std::stringstream{paramValue};
    auto stream = Stream{inputStream};
    skipWhitespace(stream, false);
    auto quotedParam = detail::readQuotedString(stream);
    if (quotedParam)
        return *quotedParam;
    else
        return paramValue;
}

} //namespace figcone::ini::detail