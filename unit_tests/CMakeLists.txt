cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
add_definitions(-DTEST_DATA_DIR=\"${TEST_DATA_DIR}\")

project(Testing LANGUAGES CXX)

find_package(OpenCL REQUIRED)
find_package(GTest REQUIRED)
find_package(OpenCLHeadersCpp CONFIG REQUIRED)
find_package(OpenCLHeaders CONFIG REQUIRED)

set(RELEASE_COMPILE_OPTIONS
	-O2
	-fstack-protector
	-fPIE
	-fstrict-overflow
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
	-ggdb3
	-O0
	-Wall
	-Wextra
	-Wc++14-compat
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
	-Wstack-protector
	-fcheck-new
	-fsized-deallocation
	-fstack-protector
	-fstrict-overflow
	-fno-omit-frame-pointer
	-Wlarger-than=8192
	-fPIE
	-Werror=vla
	-Wno-pre-c++17-compat
	-Wno-pre-c++20-compat
	# -fsanitize=address,undefined
)

add_executable(unit_tests
	src/unit_tests.cpp
	main.cpp
)

target_compile_definitions(unit_tests PRIVATE
    BITONIC_KERNEL_PATH="${CMAKE_BINARY_DIR}/bitonic_sort.cl"
)

target_compile_options(unit_tests PRIVATE
    $<$<CONFIG:Debug>:${DEBUG_COMPILE_OPTIONS}>
    $<$<CONFIG:Release>:${RELEASE_COMPILE_OPTIONS}>
)

target_compile_definitions(unit_tests PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

# target_link_options(unit_tests PRIVATE
# 	$<$<CONFIG:Debug>:-fsanitize=address,undefined>
# )

target_link_options(unit_tests PRIVATE
  $<$<CONFIG:Debug>:-Wl,--no-as-needed>
)

# target_link_libraries(unit_tests
# 	GTest::GTest
# 	GTest::Main
# 	${OpenCL_LIBRARIES}
# )

target_include_directories(unit_tests PRIVATE
	${CMAKE_SOURCE_DIR}/unit_tests/test_utils/include
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/include/detail
	${CMAKE_SOURCE_DIR}/utils
)

target_link_libraries(unit_tests PRIVATE
    GTest::GTest
    GTest::Main
    OpenCL::OpenCL
    OpenCL::HeadersCpp
    OpenCL::Headers
)

if(BS_DUMP)
    target_compile_definitions(unit_tests PRIVATE BS_DUMP)
endif()

if(BS_LOG)
    target_compile_definitions(unit_tests PRIVATE BS_LOG)
endif()

if(BS_PRINT_DURATION)
    target_compile_definitions(unit_tests PRIVATE BS_PRINT_DURATION)
endif()

if(BS_CPU)
    target_compile_definitions(unit_tests PRIVATE BS_CPU)
endif()

enable_testing()
add_test(NAME UnitTests COMMAND unit_tests)