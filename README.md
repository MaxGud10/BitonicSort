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