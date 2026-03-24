#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

#include "bitonic_cpu.h"

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

    const auto start = std::chrono::steady_clock::now();
    std::sort(vec.begin(), vec.end());
    const auto end   = std::chrono::steady_clock::now();

    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::cout << "N="           << N  << "\n";
    std::cout << "CPU_wall_ns=" << ns << "\n";
    return 0;
}
catch (std::exception &err)
{
    std::cerr << "exception: " << err.what() << '\n';
    return -1;
}