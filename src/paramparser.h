#pragma once
#include <optional>
#include <string>
#include <vector>

namespace figcone::ini::detail {
class Stream;

std::optional<std::string> readParam(
        Stream& stream,
        const std::string& wordSeparator,
        const std::vector<std::string>& paramListValue,
        const std::string& paramName);

std::optional<std::vector<std::string>> readParamList(
        const std::string& paramName,
        const std::string& paramValue);
std::string readParam(const std::string& paramValue);

}