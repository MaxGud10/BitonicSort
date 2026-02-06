#pragma once

#include <iostream>

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

    if (fabs(lhs - rhs) < fp_tolerance)
        return 0;

    else if ((lhs - rhs) > fp_tolerance)
        return 1;

    else
        return -1;
}

template <typename T> void hight_dump(const std::vector<T> &data)
{
    std::cout << std::format("{:#^{}}", "", 55) << '\n';
    for (const auto &elem : data)
    {
        std::cout << std::format("{:{}}", "", elem) << elem << '\n';
    }

    std::cout << std::format("{:#^{}}", "", 55) << '\n';
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