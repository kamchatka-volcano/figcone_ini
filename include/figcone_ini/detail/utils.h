#pragma once
#include "stream.h"
#include <figcone_tree/errors.h>
#include <string>
#include <optional>
#include <functional>

namespace figcone::ini::detail{

inline void skipWhitespace(Stream& stream, bool withNewLine = true)
{
    while(!stream.atEnd()) {
        auto nextChar = stream.peek().front();
        if (!withNewLine && nextChar == '\n')
            return;

        if (std::isspace(nextChar))
            stream.skip(1);
        else
            return;
    }
}

inline std::string readUntil(Stream& stream, std::function<bool(char)> stopPred)
{
    auto result = std::string{};
    while (!stream.atEnd()) {
        if (stopPred(stream.peek().front()))
            return result;
        result += stream.read();
    }
    return result;
}

inline std::string readWord(Stream& stream, const std::string& stopChars = {})
{
    return readUntil(stream,
                     [&stopChars](char ch) { return std::isspace(ch) || stopChars.find(ch) != std::string::npos; });
}

inline std::optional<std::string> readQuotedString(Stream& stream)
{
    if (stream.atEnd())
        return {};

    auto quotationMark = stream.peek().front();
    if (quotationMark != '\'' && quotationMark != '"')
        return {};
    auto pos = stream.position();
    stream.skip(1);

    auto result = std::string{};
    while (!stream.atEnd()) {
        auto ch = stream.read().front();
        if (ch == quotationMark)
            return result;
        result += ch;
    }
    throw ConfigError{"String isn't closed", pos};
}


}