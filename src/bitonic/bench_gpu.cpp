#include <iostream>
#include <vector>

#include "bitonic_sort.h"

int main()
try
{
    size_t N{};
    std::cin >> N;
    if (bitonic::utils::cinFailed())
        return -1;
    if (!N)
        return 0;

    std::vector<int> vec;
    vec.reserve(N);

    for (size_t i = 0; i < N; ++i)
    {
        int x{};
        std::cin >> x;
        if (bitonic::utils::cinFailed())
            return -1;

        vec.push_back(x);
    }

    bitonic::OclApp app;

    cl_ulong event_ns = 0;
    long long wall_ns = 0;
    app.bsort_timed(vec, /*incr_order=*/true, event_ns, wall_ns);

    std::cout << "N=" << N << "\n";
    std::cout << "GPU_event_ns=" << event_ns << "\n";
    std::cout << "GPU_wall_ns="  << wall_ns  << "\n";
    
    return 0;
}
catch (cl::Error &err)
{
    std::cerr << "OpenCL error: " << err.err() << ": " << err.what() << '\n';
    return -1;
}
catch (std::exception &err)
{
    std::cerr << "exception: " << err.what() << '\n';
    return -1;
}