#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <gtest/gtest.h>

#include "test_utils_detail.h"

#include "log.h"
#include "bitonic_sort.h"

namespace test_utils
{

template <typename T>
void run_test(const std::string& test_name)
{
	std::string test_folder = "data";

    std::string test_path =
        std::string(TEST_DATA_DIR) + test_folder + test_name;

    std::string result = detail::get_result<T>(test_path + ".dat");
    std::string answer = detail::get_answer(test_path + ".ans");

    EXPECT_EQ(result, answer);
}

}

#endif