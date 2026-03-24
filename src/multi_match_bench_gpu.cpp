#include <cstdint>
#include <iostream>

#include "multi_match_common.h"
#include "multi_match_gpu.h"

int main()
try
{
    const bitonic::multi_match::TaskData task =
        bitonic::multi_match::TaskIo::read(std::cin);

    bitonic::multi_match::gpu::GpuMatcher matcher;
    bitonic::multi_match::gpu::GpuTimings timings;

    const bitonic::multi_match::MatchResult result =
        matcher.match_timed(task, timings);

    volatile std::uint64_t sink = 0;

    for (std::size_t i = 0; i < result.counts.size(); ++i)
        sink += result.counts[i];

    (void)sink;

    std::cout << "text_size="     << task.text_size()     << "\n";
    std::cout << "pattern_count=" << task.pattern_count() << "\n";
    std::cout << "GPU_event_ns="  << timings.event_ns     << "\n";
    std::cout << "GPU_wall_ns="   << timings.wall_ns      << "\n";

    return 0;
}
catch (const cl::Error &err)
{
    std::cerr << "OpenCL error: " << err.err() << ": " << err.what() << '\n';
    return -1;
}
catch (const std::exception &err)
{
    std::cerr << "exception: " << err.what() << '\n';
    return -1;
}