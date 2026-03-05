#pragma once

#include <iostream>
#include <cmath>

#include "log.h"

namespace bitonic
{

namespace utils
{

constexpr double fp_tolerance = 1e-8;

inline int cmp_double(double lhs, double rhs)
{
    if (std::isnan(lhs) && std::isnan(rhs))
        return 0;

    if (std::fabs(lhs - rhs) < fp_tolerance)
        return 0;

    else if ((lhs - rhs) > fp_tolerance)
        return 1;

    else
        return -1;
}

inline void hight_dump_line(char ch, std::size_t count)
{
    std::cout << std::string(count, ch) << '\n';
}

template <typename T>
void hight_dump(const std::vector<T>& vec)
{
    hight_dump_line('#', 55);

    for (const auto& elem : vec)
    {
        long long   n      =         static_cast<long long>(elem);
        std::size_t spaces = n > 0 ? static_cast<std::size_t>(n) : 0;

        std::cout << std::string(spaces, ' ') << elem << '\n';
    }

    hight_dump_line('#', 55);
}

template <typename C>
void dump(const C &container, std::ostream &out = std::cout)
{
    auto it  = std::begin(container);
    auto end = std::end  (container);

    if (it != end)
    {
        out << *it;
        ++it;
    }

    while (it != end)
    {
        out << ' ' << *it;
        ++it;
    }

    out << '\n';
}

bool cinFailed()
{
    if (std::cin.eof())
        std::cerr << "EOF reached\n"                << std::endl;

    else if (std::cin.fail())
        std::cerr << "Invalid input\n"              << std::endl;

    else if (std::cin.bad())
        std::cerr << "Fatal standart input error\n" << std::endl;

    else
        return false;

    return true;
}

}; // namespace utils

}; // namespace bitonic