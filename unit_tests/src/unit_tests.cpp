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

TEST(bd_tests, test_1000000)
{
    test_utils::run_test<double>("/bd_tests/test_1000000");
}

TEST(bd_tests, test_10000000)
{
    test_utils::run_test<double>("/bd_tests/test_10000000");
}

#endif

TEST(multi_match_common, test_1000)
{
    test_utils::run_multi_match_case_from_files("common/test_1000");
}

TEST(multi_match_common, test_1500)
{
    test_utils::run_multi_match_case_from_files("common/test_1500");
}

TEST(multi_match_common, test_15000)
{
    test_utils::run_multi_match_case_from_files("common/test_15000");
}

TEST(multi_match_common, cpu_fast_vs_ref_1000)
{
    test_utils::run_multi_match_case_cpu_fast_vs_ref("common/test_1000");
}

TEST(multi_match_common, cpu_fast_vs_ref_1500)
{
    test_utils::run_multi_match_case_cpu_fast_vs_ref("common/test_1500");
}

TEST(multi_match_common, gpu_vs_cpu_1000)
{
    test_utils::run_multi_match_case_gpu_vs_cpu("common/test_1000");
}

TEST(multi_match_common, gpu_vs_cpu_1500)
{
    test_utils::run_multi_match_case_gpu_vs_cpu("common/test_1500");
}

#ifdef BS_BIGTESTS

TEST(multi_match_bd_tests, test_100000)
{
    test_utils::run_multi_match_case_from_files("bd_tests/test_100000");
}

TEST(multi_match_bd_tests, test_1000000)
{
    test_utils::run_multi_match_case_from_files("bd_tests/test_1000000");
}

TEST(multi_match_bd_tests, test_10000000)
{
    test_utils::run_multi_match_case_from_files("bd_tests/test_10000000");
}

#endif