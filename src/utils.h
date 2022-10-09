#pragma once
#include <string>
#include <optional>
#include <functional>

namespace figcone::ini::detail{
class Stream;

void skipWhitespace(Stream& stream, bool withNewLine = true);
std::string readUntil(Stream& stream, std::function<bool(char)> stopPred);
std::string readWord(Stream& stream, const std::string& stopChars = {});
std::optional<std::string> readQuotedString(Stream& stream);

}
