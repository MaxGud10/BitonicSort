include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(unit_tests
  src/unit_tests.cpp
  main.cpp
)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
target_compile_definitions(unit_tests PRIVATE
  TEST_DATA_DIR="${TEST_DATA_DIR}"
)

target_compile_definitions(unit_tests PRIVATE
  BITONIC_KERNEL_PATH="${CMAKE_BINARY_DIR}/bitonic_sort.cl"
)

target_compile_definitions(unit_tests PRIVATE
  MULTI_MATCH_KERNEL_PATH="${CMAKE_BINARY_DIR}/multi_match.cl"
)

target_include_directories(unit_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/unit_tests/test_utils/include
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/utils
)

target_link_libraries(unit_tests PRIVATE
  GTest::gtest
  OpenCL::OpenCL
  OpenCLHeadersCpp
  multi_match_lib
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
if(BS_BIGTESTS)
  target_compile_definitions(unit_tests PRIVATE BS_BIGTESTS)
endif()

add_test(NAME UnitTests COMMAND $<TARGET_FILE:unit_tests>)