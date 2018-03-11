cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories(argo)
include_directories(Catch2/single_include)

find_package(Boost 1.66.0)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(test_foo examples/test_foo.cpp examples/foo.cpp)
add_executable(test1 tests/test1.cpp)
add_executable(test2 tests/test2.cpp)
add_executable(test3 tests/test3.cpp)
add_executable(test4 tests/test4.cpp)

