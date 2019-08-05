#include <gtest/gtest.h>

#include <jvm/Global.hpp>

TEST(EnvironmentTestCase, VariablesTest
)
{
EXPECT_NE(nullptr, getenv("COLDSPOT_JDK"));
}