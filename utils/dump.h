#pragma once
#include <cstddef>
#include <cstdio>
#include <vector>
#include <algorithm>

namespace bitonic::dump
{

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

inline void step_dump(StepState& st,
                      const char* file, int line, const char* func,
                      const char* label,
                      const std::vector<int>& before,
                      const std::vector<int>& after,
                      int  stage = -1, int  pass = -1,
                      long a     = -1, long b    = -1,
                      bool swapped = false)
{
    const size_t n = (st.head == 0) ? after.size() : std::min(st.head, after.size());

    print_header(file, line, func);

    if (stage >= 0 && pass >= 0)
        std::printf("STEP %zu: %s | stage=%d pass=%d\n", st.step, label, stage, pass);
    else
        std::printf("STEP %zu: %s\n", st.step, label);

    print_idx_line(n);
    print_vec_line("before", before, n);
    print_vec_line("after",  after,  n);
    print_diff(before, after, n);

    if (a >= 0 && b >= 0 && (size_t)a < n && (size_t)b < n)
    {
        std::printf("pair : (%ld,%ld)%s\n", a, b, swapped ? " swap" : "");
        print_marks(n, (size_t)a, (size_t)b);
    }

    std::printf("------------------------------------------------\n");

    ++st.step;

    if (st.pause)
    {
        std::printf("press Enter...\n");
        (void)std::getchar();
    }
}

} // namespace bitonic::dump
