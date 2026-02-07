#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#include "log.h"
#include "utils.h"

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#endif

#define CL_HPP_ENABLE_EXCEPTIONS

// TODO: написать CI
// TODO: написать README
// TODO: написать dump
// TODO: разбить тесты на .ans and .dat

#include <CL/opencl.hpp>

namespace bitonic
{

class OclApp
{
private:
    cl::Device       device_;
    cl::Platform     platform_;
    cl::Context      context_;
    cl::CommandQueue queue_;
    //cl::Program      prog_;

    cl::Kernel global_bmerge_;
    cl::Kernel local_bsort;

    size_t work_gr_sz_{};

    //using bsort_t = cl::KernelFunctor<cl::Buffer, int>;

    void execute_kernel (const cl::Kernel &kernel,     size_t                 global_size,
                               size_t     local_size,  std::vector<cl::Event> &events)
    {
        cl::Event event;

        int err_num = queue_.enqueueNDRangeKernel(kernel,
                                                  cl::NullRange,
                                                  cl::NDRange(global_size),
                                                  cl::NDRange(local_size),
                                                  nullptr, &event);

        if (err_num != CL_SUCCESS)
            throw std::runtime_error{"execute_kernel failed\n"};

        events.push_back(event);
    }

    void selec_device()
    {
        std::vector<cl::Platform> platforms;
        cl ::Platform::get(&platforms);

        for (auto &&pl_devices : platforms)
        {
            std::vector<cl::Device> devices;
            pl_devices.getDevices(CL_DEVICE_TYPE_GPU, &devices);
            if (devices.empty())
                pl_devices.getDevices(CL_DEVICE_TYPE_CPU, &devices);

            auto cond = [](const cl::Device &dev)
            {
                return dev.getInfo<CL_DEVICE_AVAILABLE>() &&
                       dev.getInfo<CL_DEVICE_COMPILER_AVAILABLE>();
            };

            auto dev_iter = std::find_if(devices.begin(), devices.end(), cond);

            if (dev_iter != devices.end())
            {
                device_ = *dev_iter;
                return;
            }
        }

        throw std::runtime_error{"No suiting devises found.\n"};
    }

    bool is_pow2(size_t x) { return x && ((x & (x - 1)) == 0); }

    size_t next_pow2(size_t x)
    {
        if (x <= 1)
            return 1;

        --x;

        for (size_t i = 1; i < sizeof(size_t) * 8; i <<= 1)
            x |= x >> i;

        return x + 1;
    }

    void round_up_vector(std::vector<int> &vec, bool incr_order)
    {
        const int filler = incr_order ? std::numeric_limits<int>::max()
                                      : std::numeric_limits<int>::min();

        const size_t old_size = vec.size();
        const size_t new_size = is_pow2(old_size) ? old_size : next_pow2(old_size);

        vec.resize(new_size);
        std::fill (vec.begin() + old_size, vec.end(), filler);
    }

    void load_kernels(const std::string &file_name)
    {
        std::ifstream file(file_name);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open kernel file: " + file_name);
        }

        std::string kernels_src((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());

        auto sources = cl::Program::Sources{kernels_src};
        auto prog    = cl::Program(context_, sources);

        try { prog.build(); }

        catch (cl::Error &error)
        {
            std::cerr << error.what() << std::endl;
            std::cerr << prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device_) << "\n";
            throw;
        }

        global_bmerge_ = cl::Kernel(prog, "global_bmerge");
        local_bsort    = cl::Kernel(prog, "local_bsort");
    }

    void count_time (std::vector<cl::Event> &events, cl_ulong *duration)
    {
        for (auto &&evnt : events)
        {
            const auto start =
                evnt.getProfilingInfo<CL_PROFILING_COMMAND_START>();

            const auto end =
                evnt.getProfilingInfo<CL_PROFILING_COMMAND_END>();

            const auto evnt_duration = end - start;

            *duration += evnt_duration;
        }
    }

public:
    OclApp()
    {
        selec_device();

        work_gr_sz_ = device_.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

        context_ = cl::Context{device_};
        queue_   = cl::CommandQueue{context_, device_, CL_QUEUE_PROFILING_ENABLE};

        load_kernels(BITONIC_KERNEL_PATH);

        // try
        // {
        //     load_kernels("bitonic_sort.cl");         // ./bitonic_sort
        // }
        // catch (...)
        // {
        //     load_kernels("kernels/bitonic_sort.cl"); // ./build/bitonic_sort
        // }
    }

    void bsort(std::vector<int> &vec, bool incr_order)
    {
        const size_t old_size = vec.size();
        if (old_size == 0) return;

        round_up_vector(vec, incr_order);

        const size_t glob_size = vec.size() / 2;

        const size_t max_wg_size = device_.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
        const size_t local_mem   = device_.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
        const size_t max_by_lmem = local_mem / (2 * sizeof(int));

        LOG("glob_size: {}\nmax_wg_size: {}\nlocal_mem: {}\nmax_by_lmem: {}\n",
            glob_size, max_wg_size, local_mem, max_by_lmem);

        size_t loc_size = std::min({glob_size, max_wg_size, max_by_lmem});
        while (loc_size > 1 && (glob_size % loc_size) != 0)
            --loc_size;

        if (loc_size < 2)
            throw std::runtime_error(":( choose valid local_size (became < 2) :(");

        LOG("loc_size: {}\n", loc_size);

        const size_t nbytes = sizeof(int) * vec.size();

        cl::Buffer glob_buf(context_, CL_MEM_READ_WRITE, nbytes);

        queue_.enqueueWriteBuffer(glob_buf, CL_TRUE, 0, nbytes, vec.data());

        const uint pair_amount = static_cast<uint>(std::ceil(std::log2(static_cast<double>(vec.size()))));

        uint cur_stage = static_cast<uint>(std::log2((double)loc_size));

        cl::LocalSpaceArg loc_buf = cl::Local(2 * loc_size * sizeof(int));

#ifdef BS_PRINT_DURATION
        cl_ulong duration = 0;
#endif // BS_PRINT_DURATION
        std::vector<cl::Event> events;

        local_bsort.setArg(0, glob_buf);
        local_bsort.setArg(1, cur_stage);
        local_bsort.setArg(2, loc_buf);
        local_bsort.setArg(3, static_cast<unsigned>(incr_order));

        execute_kernel(local_bsort, glob_size, loc_size, events);

        events.back().wait();

#ifdef BS_PRINT_DURATION
        count_time(events, &duration);
#endif // BS_PRINT_DURATION

        events.clear();

        for (; cur_stage < pair_amount; ++cur_stage)
        {
            for (uint passed_stage = 0; passed_stage < cur_stage + 1; ++passed_stage)
            {
                global_bmerge_.setArg(0, glob_buf);
                global_bmerge_.setArg(1, cur_stage);
                global_bmerge_.setArg(2, passed_stage);
                global_bmerge_.setArg(3, static_cast<unsigned>(incr_order));

                execute_kernel(global_bmerge_, glob_size, loc_size, events);
            }
        }

        for (auto &&evnt : events)
            evnt.wait();

#ifdef BS_PRINT_DURATION
        count_time(events, &duration);
#endif // BS_PRINT_DURATION

        cl::copy(queue_, glob_buf, vec.begin(), vec.end());

#ifdef BS_PRINT_DURATION
        std::cout << "GPU Duration: " << duration << " nanosec\n";
#endif

        vec.resize(old_size);
    }
};

}; // namespace bitonic