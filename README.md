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
1. Создадим виртуальную среду и установим Conan:
```bash
python3 -m venv .venv && source .venv/bin/activate && pip3 install conan
```


2. Установим зависимости проекта с помощью Conan:
```bash
conan install . --output-folder=third_party -s build_type=Release --build=missing
```

3. Создаем проект:
```bash
cmake -S ./ -B build/ -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### `Режим Debug`

1. Создадим виртуальную среду и установим Conan:

```bash
python3 -m venv .venv && source .venv/bin/activate && pip3 install conan
```
2. Установим зависимости проекта с помощью Conan:
```bash
conan install . --output-folder=third_party -s build_type=Debug --build=missing
```
3. Создаем проект
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