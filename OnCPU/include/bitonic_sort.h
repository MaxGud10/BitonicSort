#pragma once

#include <iostream>
#include <stack>
#include <iterator>
#include <utility>
#include <algorithm>
#include "utils.h"

namespace bitonic
{

template <typename C>
void Merge(C& arr, size_t lhs_id, size_t block_size, bool ascending)
{
    for (size_t k = block_size / 2; k > 0; k /= 2)
		for (size_t i = lhs_id; i + k < lhs_id + block_size; ++i)
            if ((arr[i] > arr[i + k]) == ascending)
				std::swap(arr[i], arr[i + k]);
}

template <typename C>
void Sort(C& data)
{
    size_t data_size = data.size();
    for (size_t block_size = 2; block_size <= data_size; block_size *= 2)
	{
        for (size_t lhs_id = 0; lhs_id < data_size; lhs_id += block_size)
		{
            bool is_ascending = ((lhs_id / block_size) % 2 == 0);
            Merge(data, lhs_id, block_size, is_ascending);
        }

		utils::hight_dump(data);
    }
}

}; // bitonic