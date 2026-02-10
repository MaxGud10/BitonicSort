#include "bitonic_sort.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

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

    for ([[maybe_unused]] auto _ : std::views::iota(size_t(0), N))
    {
        int elem{};

        std::cin >> elem;
        if (bitonic::utils::cinFailed())
            return -1;

        vec.push_back(elem);
    }

#ifdef ENABLE_LOGGING
    std::cout << "Initial data:\n";
    bitonic::utils::dump(vec);
#endif

#ifdef BS_CPU
    std::vector<int> vec_copy(vec);

    auto start = std::chrono::high_resolution_clock::now();
    std::sort(vec_copy.begin(), vec_copy.end());
    auto end   = std::chrono::high_resolution_clock::now();

    [[maybe_unused]] auto duration = end - start;

#ifdef PRINT_DURATION
    std::cout << "CPU Duration: " << duration.count() << " nanosec\n";
#endif // PRINT_DURATION

#endif // BS_CPU

    bitonic::OclApp app;

    app.bsort(vec, /* incr_order = */ true);

    MSG("Sorted data:\n");

    bitonic::utils::dump(vec);

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