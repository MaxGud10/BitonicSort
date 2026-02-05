#include "bitonic_sort.h"
#include <iostream>
#include <vector>
#include <random>

int main()
try
{
    OclApp app;

	auto nelems = static_cast<size_t>(1) << 10;

    cl::vector<cl_int> data;
	data.reserve(nelems);

	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 50);

    for (size_t id = 0; id < nelems; ++id) {
        data.push_back(dis(gen));
    }

	std::cout << "INITIAL:\n";
    bitonic::utils::hight_dump(data);

    app.bsort(data.data(), data.size());

	std::cout << "SORTED:\n";
    bitonic::utils::hight_dump(data);
}
catch (cl::Error &err)
{
    std::cerr << "OpenCL error: " << err.err() << ":" << err.what()
              << std::endl;
    return -1;
}
catch (std::runtime_error &err)
{
    std::cerr << "runtime error: " << err.what() << std::endl;
    return -1;
}
catch (...)
{
    std::cerr << "unknown error\n";
    return -1;
}