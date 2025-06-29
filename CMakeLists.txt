cmake_minimum_required(VERSION 3.20)
project(custom-containers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CONTAINER_HEADERS inc)
add_library(custom_containers INTERFACE)
target_include_directories(custom_containers INTERFACE inc)

if (NOT CUSTOM_CONTAINERS_TESTS_DISABLED)
    add_subdirectory(ext/googletest)
    file(GLOB_RECURSE TESTS tests/*.cpp)

    add_executable(custom_containers_test ${TESTS})
    target_link_libraries(custom_containers_test
            PRIVATE
            custom_containers
            gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(custom_containers_test)
endif ()

if (NOT CUSTOM_CONTAINERS_BENCHMARK_DISABLED)
    add_subdirectory(ext/benchmark)

    file(GLOB_RECURSE BENCHMARKS benchmarks/*.cpp)

    add_executable(custom_containers_benchmark ${BENCHMARKS})
    target_link_libraries(custom_containers_benchmark
            PRIVATE
            custom_containers
            benchmark::benchmark
            benchmark::benchmark_main
    )
endif()