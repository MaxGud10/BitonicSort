# Bitonic Sort

### О проекте

Результат работы Bitonic Sort на GPU дерева сравнивается в результатом работы std::sort на CPU.

## Запуск проекта
```
git clone https://github.com/MaxGud10/BitonicSort
cd BitonicSort
mkdir build
```


### `Pежим Release`

Создаем проект:
```bash
cmake -S ./ -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### `Режим Debug`

Создаем проект
```bash
cmake -S ./ -B build/ -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```
### `Запуск`
```
./build/bitonic_sort < unit_tests/data/common/test_1000.dat
```

# `Benchmark режим`

Для измерения производительности реализованы два отдельных исполняемых файла.

-----

## `GPU benchmark`

Измеряет:
- **GPU_event_ns** - время выполнения OpenCL kernels
- **GPU_wall_ns** - общее wall-time выполнения

### Запуск
```
./build/bitonic_gpu_bench < unit_tests/data/common/test_1000.dat
```
## `CPU benchmark`

### Запуск
```
./build/bitonic_cpu_bench < unit_tests/data/common/test_1000.dat
```


### Запуск примера основной программы

1) `mkdir build`

2) `cd build`

3) `cmake ..`

4) `make`

5) `./bitonic_sort`

### Запуск тестов

1) `cmake ..`
2) `make`
3) `ctest` или `./unit_tests/unit_tests`