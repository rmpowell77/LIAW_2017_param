cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories(argo)

find_package(Boost 1.66.0)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(test_foo examples/test_foo.cpp examples/foo.cpp)


