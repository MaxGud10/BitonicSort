#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "multi_match_common.h"

#ifndef CL_HPP_TARGET_OPENCL_VERSION
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION  120
#endif

#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/opencl.hpp>

namespace bitonic
{
namespace multi_match
{
namespace gpu
{

class CompiledPatterns
{
public:
    std::vector<std::int32_t>  transitions;
    std::vector<std::uint32_t> out_offsets;
    std::vector<std::uint32_t> out_pattern_ids;
    std::vector<std::uint32_t> input_to_unique;

    std::size_t state_count_         {0};
    std::size_t unique_pattern_count_{0};
    std::size_t input_pattern_count_ {0};
};

class GpuTimings
{
public:
    cl_ulong event_ns{0};
    long long wall_ns{0};
};

class TrieNode
{
public:
    std::vector<int>           next_;
    std::vector<std::uint32_t> out_;

    TrieNode() : next_(kAlphabetSize, -1) {}
};

class PatternCompiler
{
public:
    CompiledPatterns compile(const std::vector<std::string> &input_patterns) const
    {
        CompiledPatterns compiled;
        compiled.input_pattern_count_ = input_patterns.size();

        if (input_patterns.empty())
            return compiled;

        std::vector<std::string> unique_patterns;
        unique_patterns.reserve(input_patterns.size());

        std::unordered_map<std::string, std::uint32_t> uniq_map;
        compiled.input_to_unique.reserve(input_patterns.size());

        for (std::size_t i = 0; i < input_patterns.size(); ++i)
        {
            const std::string &pat = input_patterns[i];

            if (pat.empty())
                throw std::runtime_error("Empty pattern is not allowed");

            std::unordered_map<std::string, std::uint32_t>::const_iterator it =
                uniq_map.find(pat);

            if (it == uniq_map.end())
            {
                const std::uint32_t id =
                    static_cast<std::uint32_t>(unique_patterns.size());

                uniq_map.insert(std::make_pair(pat, id));
                unique_patterns         .push_back(pat);
                compiled.input_to_unique.push_back(id);
            }
            else
                compiled.input_to_unique.push_back(it->second);
        }

        compiled.unique_pattern_count_ = unique_patterns.size();

        std::vector<TrieNode> nodes;
        nodes.push_back(TrieNode{});

        for (std::uint32_t pat_id = 0; pat_id < unique_patterns.size(); ++pat_id)
        {
            const std::string &pat = unique_patterns[pat_id];
            int state = 0;

            for (std::size_t i = 0; i < pat.size(); ++i)
            {
                const unsigned char ch = static_cast<unsigned char>(pat[i]);
                int &to = nodes[static_cast<std::size_t>(state)].next_[ch];

                if (to == -1)
                {
                    to = static_cast<int>(nodes.size());
                    nodes.push_back(TrieNode{});
                }

                state = to;
            }

            nodes[static_cast<std::size_t>(state)].out_.push_back(pat_id);
        }

        compiled.state_count_ = nodes.size();
        compiled.transitions.assign(compiled.state_count_ * kAlphabetSize, -1);

        for (std::size_t s = 0; s < nodes.size(); ++s)
        {
            for (std::size_t c = 0; c < kAlphabetSize; ++c)
                compiled.transitions[s * kAlphabetSize + c] = nodes[s].next_[c];
        }

        compiled.out_offsets.resize(compiled.state_count_ + 1, 0);

        std::size_t offset = 0;
        for (std::size_t s = 0; s < nodes.size(); ++s)
        {
            compiled.out_offsets[s] = static_cast<std::uint32_t>(offset);
            offset += nodes[s].out_.size();
        }

        compiled.out_offsets[compiled.state_count_] =
            static_cast<std::uint32_t>(offset);

        compiled.out_pattern_ids.reserve(offset);

        for (std::size_t s = 0; s < nodes.size(); ++s)
        {
            compiled.out_pattern_ids.insert(
                compiled.out_pattern_ids.end(),
                nodes[s].out_.begin(),
                nodes[s].out_.end());
        }

        return compiled;
    }
};

class GpuMatcher
{
private:
    cl::Device       device_;
    cl::Context      context_;
    cl::CommandQueue queue_;
    cl::Kernel       match_kernel_;

    void select_device()
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        for (std::size_t p = 0; p < platforms.size(); ++p)
        {
            std::vector<cl::Device> devices;
            platforms[p].getDevices(CL_DEVICE_TYPE_GPU, &devices);

            for (std::size_t d = 0; d < devices.size(); ++d)
            {
                const bool ok =
                    devices[d].getInfo<CL_DEVICE_AVAILABLE>() &&
                    devices[d].getInfo<CL_DEVICE_COMPILER_AVAILABLE>();

                if (ok)
                {
                    device_ = devices[d];
                    return;
                }
            }
        }

        throw std::runtime_error("No suitable GPU OpenCL device found");
    }

    void load_kernels(const std::string &file_name)
    {
        std::ifstream file(file_name.c_str());

        if (!file.is_open())
            throw std::runtime_error("Failed to open kernel file: " + file_name);

        std::string src(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        cl::Program program(context_, cl::Program::Sources{src});

        try
        {
            program.build({device_});
        }
        catch (const cl::Error &)
        {
            const std::string log =
                program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device_);
            throw std::runtime_error("OpenCL build failed:\n" + log);
        }

        match_kernel_ = cl::Kernel(program, "multi_match_count");
    }

public:
    GpuMatcher()
    {
        select_device();
        context_ = cl::Context(device_);
        queue_   = cl::CommandQueue(context_, device_, CL_QUEUE_PROFILING_ENABLE);
        load_kernels(MULTI_MATCH_KERNEL_PATH);
    }

    MatchResult match(const TaskData &task)
    {
        GpuTimings timings;
        return match_timed(task, timings);
    }

    MatchResult match_timed(const TaskData &task, GpuTimings &timings)
    {
        task.validate();

        PatternCompiler compiler;
        const CompiledPatterns compiled = compiler.compile(task.pattern_strings());

        return match_timed(task.text, compiled, timings);
    }

    MatchResult match(const std::string &text, const CompiledPatterns &compiled)
    {
        GpuTimings timings;
        return match_timed(text, compiled, timings);
    }

    MatchResult match_timed(const std::string      &text,
                            const CompiledPatterns &compiled,
                                  GpuTimings       &timings)
    {
        timings.event_ns = 0;
        timings.wall_ns  = 0;

        MatchResult result(compiled.input_pattern_count_);

        if (compiled.input_pattern_count_ == 0)
            return result;

        if (text.empty())
            return result;

        const std::vector<unsigned char> text_bytes(text.begin(), text.end());
        std::vector<count_type> unique_counts(compiled.unique_pattern_count_, 0);

        cl::Buffer text_buf(
            context_,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(unsigned char) * text_bytes.size(),
            const_cast<unsigned char *>(text_bytes.data()));

        cl::Buffer transitions_buf(
            context_,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(std::int32_t) * compiled.transitions.size(),
            const_cast<std::int32_t *>(compiled.transitions.data()));

        cl::Buffer out_offsets_buf(
            context_,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(std::uint32_t) * compiled.out_offsets.size(),
            const_cast<std::uint32_t *>(compiled.out_offsets.data()));

        cl::Buffer out_pattern_ids_buf(
            context_,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(std::uint32_t) * compiled.out_pattern_ids.size(),
            compiled.out_pattern_ids.empty()
                ? nullptr
                : const_cast<std::uint32_t *>(compiled.out_pattern_ids.data()));

        cl::Buffer counts_buf(
            context_,
            CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
            sizeof(count_type) * unique_counts.size(),
            unique_counts.data());

        match_kernel_.setArg(0, text_buf);
        match_kernel_.setArg(1, static_cast<cl_uint>(text_bytes.size()));
        match_kernel_.setArg(2, transitions_buf);
        match_kernel_.setArg(3, out_offsets_buf);
        match_kernel_.setArg(4, out_pattern_ids_buf);
        match_kernel_.setArg(5, counts_buf);

        cl::Event event;

        const auto wall_start = std::chrono::steady_clock::now();

        queue_.enqueueNDRangeKernel(
            match_kernel_,
            cl::NullRange,
            cl::NDRange(text_bytes.size()),
            cl::NullRange,
            nullptr,
            &event);

        event.wait();

        const auto start =
            event.getProfilingInfo<CL_PROFILING_COMMAND_START>();

        const auto end =
            event.getProfilingInfo<CL_PROFILING_COMMAND_END>();

        timings.event_ns = end - start;

        queue_.enqueueReadBuffer(
            counts_buf,
            CL_TRUE,
            0,
            sizeof(count_type) * unique_counts.size(),
            unique_counts.data());

        const auto wall_end = std::chrono::steady_clock::now();
        timings.wall_ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                wall_end - wall_start).count();

        for (std::size_t i = 0; i < compiled.input_pattern_count_; ++i)
        {
            const std::uint32_t uniq_id = compiled.input_to_unique[i];
            result.counts[i] = unique_counts[uniq_id];
        }

        return result;
    }
};

} // namespace gpu
} // namespace multi_match
} // namespace bitonic