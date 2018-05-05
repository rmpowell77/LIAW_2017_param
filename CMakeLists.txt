cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories(argo)
include_directories(Catch2/single_include)

find_package(Boost 1.66.0)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(test_foo examples/test_foo.cpp examples/foo.cpp)
add_executable(simple_example examples/simple_example.cpp)

file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} tests/*.cpp)

enable_testing()

foreach(testSrc ${TEST_SRCS})
  get_filename_component(testName ${testSrc} NAME_WE)
  add_executable(${testName} ${testSrc})
  add_test(${testName} ${testName})
endforeach(testSrc)

