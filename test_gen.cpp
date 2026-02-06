#include <iostream>
#include <random>
#include <ranges>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
        return 1;
    }

    size_t N = std::strtoul(argv[1], nullptr, 10);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist(-10000, 10000);

    std::cout << N << '\n';

    for (auto _ : std::views::iota(size_t(0), N))
    {
        std::cout << dist(gen) << '\n';
    }

    return 0;
}