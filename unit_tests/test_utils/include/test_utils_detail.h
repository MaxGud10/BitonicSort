#ifndef TEST_UTILS_DETAIL
#define TEST_UTILS_DETAIL

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "bitonic_sort.h"
#include "multi_match_common.h"
#include "multi_match_cpu.h"
#include "multi_match_gpu.h"
#include "log.h"
#include "utils.h"

namespace test_utils
{

namespace detail
{

template <typename T>
std::string get_result(const std::string &file_name)
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

    for (std::size_t i = 0; i < N; ++i)
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

inline std::string get_answer(const std::string &file_name)
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

namespace multi_match_detail
{

inline bitonic::multi_match::TaskData load_task(const std::string& file_name)
{
    std::ifstream in(file_name.c_str());

    if (!in.is_open())
        throw std::runtime_error("Can't open " + file_name);

    return bitonic::multi_match::TaskIo::read(in);
}

inline bitonic::multi_match::MatchResult load_answer(const std::string& file_name)
{
    std::ifstream in(file_name.c_str());

    if (!in.is_open())
        throw std::runtime_error("Can't open " + file_name);

    return bitonic::multi_match::TaskIo::read_answer(in);
}

inline bitonic::multi_match::MatchResult run_gpu(
    const bitonic::multi_match::TaskData& task)
{
    bitonic::multi_match::gpu::GpuMatcher matcher;
    return matcher.match(task);
}

inline bitonic::multi_match::MatchResult run_cpu_ref(
    const bitonic::multi_match::TaskData& task)
{
    bitonic::multi_match::cpu::CpuMatcher matcher;
    return matcher.match_reference(task);
}

inline bitonic::multi_match::MatchResult run_cpu_fast(
    const bitonic::multi_match::TaskData& task)
{
    bitonic::multi_match::cpu::CpuMatcher matcher;
    return matcher.match_fast(task);
}

} // namespace multi_match_detail

}; // namespace test_utils

#endif // TEST_UTILS_DETAILS