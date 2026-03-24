#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <gtest/gtest.h>
#include <string>

#include "test_utils_detail.h"

#include "bitonic_sort.h"
#include "log.h"

namespace test_utils
{

template <typename T>
void run_test(const std::string &test_name)
{
    std::string test_folder = "data";

    std::string test_path =
        std::string(TEST_DATA_DIR) + test_folder + test_name;

    std::string result = detail::get_result<T>(test_path + ".dat");
    std::string answer = detail::get_answer   (test_path + ".ans");

    EXPECT_EQ(result, answer);
}

inline void run_multi_match_case_from_files(const std::string& stem)
{
    const std::string base =
        std::string(TEST_DATA_DIR) + "data/multi_match/" + stem;

    const bitonic::multi_match::TaskData task =
        multi_match_detail::load_task(base + ".dat");

    const bitonic::multi_match::MatchResult expected =
        multi_match_detail::load_answer(base + ".ans");

    const bitonic::multi_match::MatchResult actual =
        multi_match_detail::run_gpu(task);

    EXPECT_TRUE(actual.equals(expected));
}

inline void run_multi_match_case_gpu_vs_cpu(const std::string& stem)
{
    const std::string base =
        std::string(TEST_DATA_DIR) + "data/multi_match/" + stem;

    const bitonic::multi_match::TaskData task =
        multi_match_detail::load_task(base + ".dat");

    const bitonic::multi_match::MatchResult gpu_res =
        multi_match_detail::run_gpu(task);

    const bitonic::multi_match::MatchResult cpu_res =
        multi_match_detail::run_cpu_ref(task);

    EXPECT_TRUE(gpu_res.equals(cpu_res));
}

inline void run_multi_match_case_cpu_fast_vs_ref(const std::string& stem)
{
    const std::string base =
        std::string(TEST_DATA_DIR) + "data/multi_match/" + stem;

    const bitonic::multi_match::TaskData task =
        multi_match_detail::load_task(base + ".dat");

    const bitonic::multi_match::MatchResult fast_res =
        multi_match_detail::run_cpu_fast(task);

    const bitonic::multi_match::MatchResult ref_res =
        multi_match_detail::run_cpu_ref(task);

    EXPECT_TRUE(fast_res.equals(ref_res));
}

} // namespace test_utils

#endif