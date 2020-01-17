cmake_minimum_required(VERSION 3.10)

project(ODDs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(ODDs STATIC
    lib/ODDs/JSONDump.cpp
    lib/ODDs/ODDs.cpp
    lib/ODDs/Operations.cpp)

target_include_directories(ODDs PUBLIC lib/)
target_include_directories(ODDs PUBLIC ext/rapidjson/include/)
target_compile_options(ODDs PRIVATE -Wall -Wextra -Wshadow)

enable_testing()

include(GoogleTest)
add_subdirectory(ext/googletest)

add_executable(ODDsTest
    test/Common/DivisionODDs.cpp
    test/DivisionODDs.cpp
    test/ODDBuilder.cpp
    test/ODDsStructures.cpp
    test/ODDsUnion.cpp
    test/ODDsIntersection.cpp
    test/ODDsTensorProduct.cpp
    test/ODDsNegation.cpp
    test/ODDsMapping.cpp
    test/ODDsPowerSet.cpp)
target_include_directories(ODDsTest PUBLIC lib/)
target_include_directories(ODDsTest PUBLIC test/)
target_include_directories(ODDsTest PUBLIC ext/googletest/googletest/include/)
target_link_libraries(ODDsTest gtest gtest_main)
target_link_libraries(ODDsTest ODDs)
target_compile_options(ODDsTest PRIVATE -Wall -Wextra -Wshadow)
gtest_discover_tests(ODDsTest)
