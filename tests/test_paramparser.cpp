#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone_ini/parser.h>
#include <sstream>

namespace test_paramparser {

auto parseParam(const std::string& str)
{
    auto input = std::stringstream{str};
    auto parser = figcone::ini::Parser{};
    return parser.parse(input);
}

TEST(TestParam, Basic)
{
    auto result = parseParam(R"( test = 1)");
    auto& tree = result.asItem();

    ASSERT_EQ(tree.paramsCount(), 1);
    EXPECT_EQ(tree.param("test").value(), "1");
}

TEST(TestParam, BasicString)
{
    auto result = parseParam(R"( test = "Hello world")");
    auto& tree = result.asItem();

    ASSERT_EQ(tree.paramsCount(), 1);
    EXPECT_EQ(tree.param("test").value(), "Hello world");
}

TEST(TestParam, EmptyStringParam)
{
    auto result = parseParam(R"( test = "")");
    auto& tree = result.asItem();

    ASSERT_EQ(tree.paramsCount(), 1);
    EXPECT_EQ(tree.param("test").value(), "");
}

}
