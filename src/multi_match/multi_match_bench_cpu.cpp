#include <chrono>
#include <cstdint>
#include <iostream>

#include "multi_match_common.h"
#include "multi_match_cpu.h"

int main()
try
{
    const bitonic::multi_match::TaskData task =
        bitonic::multi_match::TaskIo::read(std::cin);

    bitonic::multi_match::cpu::CpuMatcher matcher;

    const auto start = std::chrono::steady_clock::now();
    const bitonic::multi_match::MatchResult result = matcher.match_reference(task);
    const auto end   = std::chrono::steady_clock::now();

    const auto ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    volatile std::uint64_t sink = 0;

    for (std::size_t i = 0; i < result.counts.size(); ++i)
        sink += result.counts[i];

    (void)sink;

    std::cout << "text_size="     << task.text_size()     << "\n";
    std::cout << "pattern_count=" << task.pattern_count() << "\n";
    std::cout << "CPU_wall_ns="   << ns                   << "\n";

    return 0;
}
catch (const std::exception &err)
{
    std::cerr << "exception: " << err.what() << '\n';
    return -1;
}