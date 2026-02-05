#include <algorithm>
#include <format>
#include <iostream>
#include <vector>

#include "bitonic_sort.h"
#include "utils.h"

int main()
{
    std::vector<int> data{3, 2, 13, 5, 9, 5, 6, 3, 14, 11, 5, 3, 5, 6, 17, 19};
    std::cout << "data size: " << data.size() << '\n';

	std::cout << "INITIAL:\n";
    bitonic::utils::hight_dump(data);

    bitonic::Sort(data);

	std::cout << "RESULT:\n";
    bitonic::utils::hight_dump(data);

    return 0;
}