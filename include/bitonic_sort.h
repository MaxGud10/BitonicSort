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

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_ENABLE_EXCEPTIONS

#ifndef CL_MAKE_VERSION
#define CL_MAKE_VERSION(major, minor, patch) (((major) << 22) | ((minor) << 12) | (patch))
#endif

#ifdef CL_KHR_COMMAND_BUFFER_EXTENSION_VERSION
#undef CL_KHR_COMMAND_BUFFER_EXTENSION_VERSION
#define CL_KHR_COMMAND_BUFFER_EXTENSION_VERSION 0
#endif

#ifdef CL_KHR_COMMAND_BUFFER_MUTABLE_DISPATCH_EXTENSION_VERSION
#undef CL_KHR_COMMAND_BUFFER_MUTABLE_DISPATCH_EXTENSION_VERSION
#define CL_KHR_COMMAND_BUFFER_MUTABLE_DISPATCH_EXTENSION_VERSION 0
#endif

#ifdef CL_KHR_COMMAND_BUFFER_VERSION
#undef CL_KHR_COMMAND_BUFFER_VERSION
#define CL_KHR_COMMAND_BUFFER_VERSION 0
#endif


#include <CL/opencl.hpp>

namespace bitonic
{
constexpr size_t LOCAL_SIZE = 0;
 
class OclApp
{
private:
    cl::Device       device_;
    cl::Platform     platform_;
    cl::Context      context_;
    cl::CommandQueue queue_;
    cl::Program      prog_;

    cl::Kernel global_bmerge_;
    cl::Kernel local_bsort;

    size_t work_gr_sz_{};

    using bsort_t = cl::KernelFunctor<cl::Buffer, int>;
    
    void execute_kernel (cl::Kernel kernal, size_t global_size, 
                         size_t local_size, std::vector<cl::Event> &events)
    {
        cl::Event event;

        int err_num = queue_.enqueueNDRangeKernel(kernal,      cl::NullRange, 
                                                  global_size, local_size,
                                                  nullptr,     &event);

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

    void round_up_vector(std::vector<int> &vec, bool incr_order)
    {
        int filler = incr_order ? std::numeric_limits<int>::max() 
                                : std::numeric_limits<int>::min();

        size_t old_size = vec.size();
        size_t new_size = std::pow(2, 1 + static_cast<int>(log2(old_size)));

        vec.resize(new_size);

        auto begin = vec.begin();

        std::fill(begin + old_size, begin + new_size, filler);
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

        load_kernels("../kernels/bitonic_sort.cl");
    }

    void bsort(std::vector<int> &vec, bool incr_order)
    {
        size_t old_size = vec.size();

        round_up_vector(vec, incr_order);

        size_t glob_size = vec.size() / 2;

        auto max_wg_size = device_.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

        LOG("glob_size: {}\n max_wg_size: {}\n", glob_size, max_wg_size);

        size_t loc_size = std::min(glob_size, max_wg_size);

        LOG("loc_size: {}\n", loc_size);

        auto nbytes = sizeof(int) * vec.size();

        cl::Buffer glob_buf(context_, CL_MEM_READ_WRITE, nbytes);

        queue_.enqueueWriteBuffer(glob_buf, CL_TRUE, 0, nbytes, vec.data());

        const auto pair_amount = std::ceil(std::log2(vec.size()));

        uint cur_stage = std::log2(loc_size);

        cl::LocalSpaceArg loc_buf = cl::Local(2 * loc_size * sizeof(int));

#ifdef PRINT_DURATION
        cl_ulong duration = 0;
#endif // PRINT_DURATION
        std::vector<cl::Event> events;

        local_bsort.setArg(0, glob_buf);
        local_bsort.setArg(1, cur_stage);
        local_bsort.setArg(2, loc_buf);
        local_bsort.setArg(3, static_cast<unsigned>(incr_order));

        execute_kernel(local_bsort, glob_size, loc_size, events);

        events[0].wait();

#ifdef PRINT_DURATION
        count_time(events, &duration);
#endif // PRINT_DURATION

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

#ifdef PRINT_DURATION
        count_time(events, &duration);
#endif // PRINT_DURATION

        cl::copy(queue_, glob_buf, vec.begin(), vec.end());

#ifdef PRINT_DURATION
        std::cout << "GPU Duration: " << duration << " nanosec\n";
#endif

        vec.resize(old_size);
    }
};

}; // namespace bitonic