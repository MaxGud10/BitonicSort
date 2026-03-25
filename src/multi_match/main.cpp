#include <iostream>

#include "multi_match_common.h"
#include "multi_match_gpu.h"

int main()
try // TODO: сделать нормальное README
{
    const bitonic::multi_match::TaskData task =
        bitonic::multi_match::TaskIo::read(std::cin);

    bitonic::multi_match::gpu::GpuMatcher matcher;
    const bitonic::multi_match::MatchResult result = matcher.match(task);

    bitonic::multi_match::TaskIo::write(result, std::cout);

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