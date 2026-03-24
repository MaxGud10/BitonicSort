cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0091 NEW)
project(BitonicSort LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  OpenCLHeaders
  GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-Headers.git
  GIT_TAG        v2023.12.14
)
FetchContent_GetProperties(OpenCLHeaders)
if(NOT OpenCLHeaders_POPULATED)
  FetchContent_Populate(OpenCLHeaders)
endif()

FetchContent_Declare(
  OpenCLCLHPP
  GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-CLHPP.git
  GIT_TAG        v2023.12.14
)
FetchContent_GetProperties(OpenCLCLHPP)
if(NOT OpenCLCLHPP_POPULATED)
  FetchContent_Populate(OpenCLCLHPP)
endif()

add_library(OpenCLHeadersCpp INTERFACE)
target_include_directories(OpenCLHeadersCpp INTERFACE
  ${OpenCLHeaders_SOURCE_DIR}
  ${OpenCLHeaders_SOURCE_DIR}/CL
  ${OpenCLCLHPP_SOURCE_DIR}/include
)

enable_testing()

option(BS_LOG             "Enable logging" OFF)
option(BS_PRINT_DURATION  "Print kernel duration" OFF)
option(BS_CPU             "Enable CPU reference/fallback" OFF)
option(BS_BIGTESTS        "Enable large/slow tests" OFF)
option(BS_DUMP            "Enable debug dumps" OFF)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    find_package(OpenCL CONFIG REQUIRED)
else()
    find_package(OpenCL REQUIRED)
endif()

# set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Build type")


configure_file(${CMAKE_SOURCE_DIR}/kernels/bitonic_sort.cl
               ${CMAKE_BINARY_DIR}/bitonic_sort.cl
               COPYONLY)

configure_file(${CMAKE_SOURCE_DIR}/kernels/multi_match.cl
               ${CMAKE_BINARY_DIR}/multi_match.cl
               COPYONLY)

if(MSVC)
    set(RELEASE_COMPILE_OPTIONS /O2 /W4 /DNDEBUG)
    set(DEBUG_COMPILE_OPTIONS   /Od /RTC1 /W4 /D_DEBUG)

else()
    set(RELEASE_COMPILE_OPTIONS
        -O2
        -Wformat-security
        -Wformat=2
        -Wall
        -Wextra
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wcast-align
        -Wpointer-arith
        -Wunreachable-code
        -Wno-pre-c++17-compat
    )

    set(DEBUG_COMPILE_OPTIONS
        -D_DEBUG
        -O0
        -Wall
        -Wextra
        -Wmissing-declarations
        -Wcast-align
        -Wcast-qual
        -Wchar-subscripts
        -Wconversion
        -Wctor-dtor-privacy
        -Wempty-body
        -Wfloat-equal
        -Wformat-security
        -Winline
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wpacked
        -Wpointer-arith
        -Winit-self
        -Wredundant-decls
        -Wshadow
        -Wsign-conversion
        -Wsign-promo
        -Wstrict-overflow=2
        -Wsuggest-override
        -Wswitch-default
        -Wswitch-enum
        -Wundef
        -Wunreachable-code
        -Wunused
        -Wvariadic-macros
        -Wno-missing-field-initializers
        -Wno-narrowing
        -Wno-old-style-cast
        -Wno-varargs
        -Werror=vla
        # -fsanitize=address,undefined
    )

    # Linux/Unix-only hardening flags
    if(NOT WIN32)
        list(APPEND RELEASE_COMPILE_OPTIONS
            -fstack-protector
            -fstrict-overflow
        )
        list(APPEND DEBUG_COMPILE_OPTIONS
            -fstack-protector
            -fstrict-overflow
            -Wlarger-than=8192
        )
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND RELEASE_COMPILE_OPTIONS
        -Wno-pre-c++17-compat
    )
    list(APPEND DEBUG_COMPILE_OPTIONS
        -Wc++14-compat
        -Wno-pre-c++17-compat
        -Wno-pre-c++20-compat
    )
endif()

add_library(multi_match_lib INTERFACE)

target_include_directories(multi_match_lib INTERFACE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/utils
)

target_link_libraries(multi_match_lib INTERFACE
  OpenCL::OpenCL
  OpenCLHeadersCpp
)

target_compile_definitions(multi_match_lib INTERFACE
  MULTI_MATCH_KERNEL_PATH="${CMAKE_BINARY_DIR}/multi_match.cl"
)


add_executable(bitonic_sort
    src/bitonic/main.cpp
)

add_executable(bitonic_gpu_bench
    src/bitonic/bench_gpu.cpp
)

add_executable(bitonic_cpu_bench
    src/bitonic/bench_cpu.cpp
)

add_executable(multi_match
    src/multi_match/main.cpp
)

add_executable(multi_match_gpu_bench
    src/multi_match/multi_match_bench_gpu.cpp
)

add_executable(multi_match_cpu_bench
    src/multi_match/multi_match_bench_cpu.cpp
)

foreach(tgt IN ITEMS bitonic_sort bitonic_gpu_bench)
  target_include_directories(${tgt} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/utils
  )

  target_link_libraries(${tgt} PRIVATE
    OpenCL::OpenCL
    OpenCLHeadersCpp
  )

  target_compile_definitions(${tgt} PRIVATE
    BITONIC_KERNEL_PATH="${CMAKE_BINARY_DIR}/bitonic_sort.cl"
  )

  target_compile_options(${tgt} PRIVATE
    $<$<CONFIG:Debug>:${DEBUG_COMPILE_OPTIONS}>
    $<$<CONFIG:Release>:${RELEASE_COMPILE_OPTIONS}>
  )

  target_compile_definitions(${tgt} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
  )
endforeach()

target_include_directories(bitonic_cpu_bench PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/utils
)

target_compile_options(bitonic_cpu_bench PRIVATE
  $<$<CONFIG:Debug>:${DEBUG_COMPILE_OPTIONS}>
  $<$<CONFIG:Release>:${RELEASE_COMPILE_OPTIONS}>
)

target_compile_definitions(bitonic_cpu_bench PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)


target_link_libraries(multi_match PRIVATE multi_match_lib)
target_link_libraries(multi_match_gpu_bench PRIVATE multi_match_lib)
target_link_libraries(multi_match_cpu_bench PRIVATE multi_match_lib)

target_compile_options(multi_match PRIVATE
  $<$<CONFIG:Debug>:${DEBUG_COMPILE_OPTIONS}>
  $<$<CONFIG:Release>:${RELEASE_COMPILE_OPTIONS}>
)

target_compile_options(multi_match_gpu_bench PRIVATE
  $<$<CONFIG:Debug>:${DEBUG_COMPILE_OPTIONS}>
  $<$<CONFIG:Release>:${RELEASE_COMPILE_OPTIONS}>
)

target_compile_options(multi_match_cpu_bench PRIVATE
  $<$<CONFIG:Debug>:${DEBUG_COMPILE_OPTIONS}>
  $<$<CONFIG:Release>:${RELEASE_COMPILE_OPTIONS}>
)

target_compile_definitions(multi_match PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

target_compile_definitions(multi_match_gpu_bench PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

target_compile_definitions(multi_match_cpu_bench PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)


if(BS_LOG)
    target_compile_definitions(bitonic_sort PRIVATE BS_LOG)
endif()

if(BS_CPU)
    target_compile_definitions(bitonic_sort PRIVATE BS_CPU)
endif()

if(BS_PRINT_DURATION)
    target_compile_definitions(bitonic_sort PRIVATE BS_PRINT_DURATION)
endif()

if(BS_DUMP)
    target_compile_definitions(bitonic_sort PRIVATE BS_DUMP)
endif()


target_compile_definitions(bitonic_sort PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

add_subdirectory(unit_tests/)