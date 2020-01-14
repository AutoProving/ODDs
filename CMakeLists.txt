cmake_minimum_required(VERSION 3.10)

project(ODDs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(ODDs STATIC
    lib/ODDs/ODDs.cpp)

target_include_directories(ODDs PUBLIC lib/)
target_compile_options(ODDs PRIVATE -Wall -Wextra -Wshadow)

enable_testing()

include(GoogleTest)
add_subdirectory(ext/googletest)

add_executable(ODDsTest
    test/ODDBuilder.cpp
    test/ODDsStructures.cpp)
target_include_directories(ODDsTest PUBLIC lib/)
target_link_libraries(ODDsTest gtest gtest_main)
target_link_libraries(ODDsTest ODDs)
target_compile_options(ODDsTest PRIVATE -Wall -Wextra -Wshadow)
gtest_discover_tests(ODDsTest)
