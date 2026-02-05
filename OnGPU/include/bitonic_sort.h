#pragma once

#include <algorithm>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>
#include <random>
#include <fstream>

#include "utils.h"
#include "log.h"

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#include "opencl.hpp"

constexpr size_t ARR_SIZE = 64;
constexpr size_t LOCAL_SIZE = 4;

class OclApp
{
    cl::Platform platform_;
    cl::Context context_;
    cl::CommandQueue queue_;

    using bsort_t = cl::KernelFunctor<cl::Buffer, int>;

    static cl::Platform select_platform()
    {
        cl::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        for (auto p : platforms)
        {
            cl_uint ndevices = 0;
            ::clGetDeviceIDs(p(), CL_DEVICE_TYPE_GPU, 0, NULL, &ndevices);
            if (ndevices > 0)

                return cl::Platform(
                    p);
        }

        throw std::runtime_error("Не выбрано ни одной платформы");
    }

    static cl::Context get_gpu_context(cl_platform_id PId)
	{
		cl_context_properties properties[] = {
			CL_CONTEXT_PLATFORM,
			reinterpret_cast<cl_context_properties>(PId),
			0
		};

		return cl::Context(CL_DEVICE_TYPE_GPU, properties);
	}

  public:
    OclApp()
        : platform_(select_platform())
        , context_(get_gpu_context(platform_()))
    {
		auto devices = context_.getInfo<CL_CONTEXT_DEVICES>();
		cl::Device device = devices.front();
		queue_ = cl::CommandQueue(context_, device, CL_QUEUE_PROFILING_ENABLE);

        cl::string name = platform_.getInfo<CL_PLATFORM_NAME>();
        cl::string profile = platform_.getInfo<CL_PLATFORM_PROFILE>();
        dbgs << "Выбрано: " << name << ": " << profile << std::endl;
    }

	static std::string load_kernel(const std::string& file_name)
	{
		std::ifstream file(file_name);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open kernel file: " + file_name);
		}

		return std::string(	(std::istreambuf_iterator<char>(file)),
							std::istreambuf_iterator<char>());
	}

    void bsort(cl_int* data, size_t data_size)
	{
		size_t nbytes = data_size * sizeof(cl_int);

		cl::Buffer data_buf(context_, CL_MEM_READ_WRITE, nbytes);

		cl::copy(queue_, data, data + data_size, data_buf);

		dbgs << load_kernel("bitonic_sort.cl") << '\n';

		cl::Program program(context_, load_kernel("bitonic_sort.cl"), true);

		bsort_t bitonic_sort(program, "bitonic_sort");

		cl::NDRange GlobalRange(data_size);
		cl::NDRange LocalRange(LOCAL_SIZE);

		cl::EnqueueArgs Args(queue_, GlobalRange, LocalRange);

		cl::Event evt = bitonic_sort(Args, data_buf, data_size);
		evt.wait();
		auto start_time = evt.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		auto end_time = evt.getProfilingInfo<CL_PROFILING_COMMAND_END>();

		std::cout << "Duration: " << end_time - start_time << " nanosec\n";

		cl::copy(queue_, data_buf, data, data + data_size);
	}
};