#pragma once
#include <cstddef>
#include <cstdio>
#include <vector>
#include <algorithm>

#include "log.h"

namespace bitonic::dump
{

#ifdef BS_DUMP
    #define BS_DUMP_STEP(vec, stage, pass, msg) \
        ::bitonic::dump::dump_step((vec), (stage), (pass), __FILE__, __LINE__, __func__, (msg))
#else
    #define BS_DUMP_STEP(...) do{}while(0)
#endif

struct StepState
{
    size_t step  = 0;
    bool   pause = false;
    size_t head  = 32;
};

inline void print_header(const char* file, int line, const char* func)
{
    std::printf("\n\ncalled from %s:%d, %s()\n", file, line, func);
}

inline void print_vec_line(const char* name,
                           const std::vector<int> &v,
                           size_t n)
{
    std::printf("%-5s", name);
    for (size_t i = 0; i < n; ++i)
        std::printf("%4d", v[i]);

    std::printf("\n");
}

inline void print_idx_line(size_t n)
{
    std::printf("%-5s", "idx:");
    for (size_t i = 0; i < n; ++i)
        std::printf("%4zu", i);

    std::printf("\n");
}

inline void print_marks(size_t n, size_t a, size_t b)
{
    std::printf("%-5s", "mark:");
    for (size_t i = 0; i < n; ++i)
    {
        if (i == a || i == b)
            std::printf("%4s", "^");
        else
            std::printf("%4s", "");
    }

    std::printf("\n");
}

inline void print_diff(const std::vector<int>& before,
                       const std::vector<int>& after,
                       size_t n)
{
    std::printf("%-5s", "diff:");
    for (size_t i = 0; i < n; ++i)
    {
        std::printf("%4s", (before[i] != after[i]) ? "*" : "");
    }

    std::printf("\n");
}

inline void dump_step(const std::vector<int>& v,
                      int         stage, int pass,
                      const char* file,  int line,
                      const char* func,
                      const char* msg)
{
    LOG("\n================================================================================\n");
    LOG("[BITONIC DUMP] stage={} pass={}  {}\n", stage, pass, msg);
    LOG("called from {}:{} {}\n\n", file, line, func);

    LOG("idx : ");
    for (size_t i = 0; i < v.size(); ++i)
        LOG("{:3d} ", (int)i);

    LOG("\ndata: ");
    for (int x : v)
        LOG("{:3d} ", x);

    LOG("\n================================================================================\n");
}

} // namespace bitonic::dump
