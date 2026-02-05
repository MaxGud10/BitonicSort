#pragma once

#include <vector>
#include <algorithm>
#include <iostream>

namespace bitonic
{
	template <typename RandomIt>
	void Sort(RandomIt begin, RandomIt end)
	{
		size_t data_size = std::distance(begin, end);
		// std::sort(begin, end);

		if (data_size <= 1) return;


		for (size_t i = 0; i < data_size / 2; ++i)
		{
			auto lhs_iter = std::next(begin, i);
			auto rhs_iter = std::next(begin, i + data_size / 2);

			if (*lhs_iter > *rhs_iter)
			{
				std::cout << "swapping " << *lhs_iter << " and " << *rhs_iter << '\n';
				std::iter_swap(lhs_iter, rhs_iter);
			}
		}

		auto mid = std::next(begin, data_size / 2);

		Sort(begin, mid);
		Sort(mid, end);
	}
};