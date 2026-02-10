#include "bitonic_sort.h"

#include "test_utils.h"

#include <gtest/gtest.h>

TEST(common, test_1000)
{
    test_utils::run_test<double>("/common/test_1000");
}

TEST(common, test_1500)
{
    test_utils::run_test<double>("/common/test_1500");
}

TEST(common, test_15000)
{
    test_utils::run_test<double>("/common/test_15000");
}

#ifdef BS_BIGTESTS

TEST(bd_tests, test_100000)
{
    test_utils::run_test<double>("/bd_tests/test_100000");
}

TEST(bd_tests, test_500000)
{
    test_utils::run_test<double>("/bd_tests/test_500000");
}

TEST(bd_tests, test_1000000)
{
    test_utils::run_test<double>("/bd_tests/test_1000000");
}

TEST(bd_tests, test_10000000)
{
    test_utils::run_test<double>("/bd_tests/test_10000000");
}

#endif