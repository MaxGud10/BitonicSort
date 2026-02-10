#ifndef TEST_UTILS_DETAIL
#define TEST_UTILS_DETAIL

#include <fstream>
#include <ranges>
#include <sstream>

#include "bitonic_sort.h"
#include "log.h"
#include "utils.h"

namespace test_utils
{

namespace detail
{

template <typename T> std::string get_result(std::string_view file_name)
{
    std::ifstream test_data;

    test_data.open(std::string(file_name));

    if (!test_data.is_open())
    {
        std::cerr << "Can't open " << file_name << '\n';
        throw;
    }

    std::stringstream result;

    size_t N{};

    test_data >> N;

    std::vector<int> vec;
                     vec.reserve(N);

    for ([[maybe_unused]] auto _ : std::views::iota(size_t(0), N))
    {
        int elem{};

        test_data >> elem;

        vec.push_back(elem);
    }

    bitonic::OclApp app;

    app.bsort(vec, /* incr_order = */ true);

    bitonic::utils::dump(vec, result);

    return result.str();
}

inline std::string get_answer(std::string_view file_name)
{
    std::ifstream answer_file;
                  answer_file.open(std::string(file_name));

    if (!answer_file.is_open())
    {
        std::cerr << "Can't open " << file_name << '\n';
        throw;
    }

    std::string answer((std::istreambuf_iterator<char>(answer_file)),
                        std::istreambuf_iterator<char>());

    return answer;
}

}; // namespace detail

}; // namespace test_utils

#endif // TEST_UTILS_DETAILS