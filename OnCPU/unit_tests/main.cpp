#include <vector>
#include <iostream>
#include <algorithm>
#include <format>

#include "bitonic_sort.h"
#include "utils.h"

int main()
{
	std::vector<int> data {2, 4, 6, 6, 8, 9, 11, 14, 15, 18, 22, 21, 18, 11, 6, 2};
	std::cout << "data size: " << data.size() << '\n';

	std::cout << std::format("{:#^{}}", "", 40) << '\n';

	bitonic::utils::hight_dump(data);

	std::cout << std::format("{:#^{}}", "", 40) << '\n';

	bitonic::Sort(data.begin(), data.end());

	bitonic::utils::hight_dump(data);

	std::cout << std::format("{:#^{}}", "", 40) << '\n';

    return 0;
}