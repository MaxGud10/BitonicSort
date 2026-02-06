#include "bitonic_sort.h"

#include "test_utils.h"

#include <gtest/gtest.h> 

TEST(common, uniform_1000)
{
    test_utils::run_test<double>("/common/uniform_1000");
}

TEST(common, uniform_1500)
{
    test_utils::run_test<double>("/common/uniform_1500");
}
TEST(common, uniform_10000)
{
    test_utils::run_test<double>("/common/uniform_10000");
}

TEST(common, uniform_15000)
{
    test_utils::run_test<double>("/common/uniform_15000");
}

#ifdef ENABLE_BD_TESTS

TEST(bd_tests, uniform_100000)
{
    test_utils::run_test<double>("/bd_tests/uniform_100000");
}

TEST(bd_tests, uniform_500000)
{
    test_utils::run_test<double>("/bd_tests/uniform_500000");
}

TEST(bd_tests, uniform_1000000)
{
    test_utils::run_test<double>("/bd_tests/uniform_1000000");
}

TEST(bd_tests, uniform_10000000)
{
    test_utils::run_test<double>("/bd_tests/uniform_10000000");
}

#endif