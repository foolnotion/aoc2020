cmake_minimum_required(VERSION 3.11)

include(FetchContent)
include(FeatureSummary)

project(
    aoc2020 
    VERSION 0.1
    DESCRIPTION "Advent of Code 2020"
    LANGUAGES CXX
)

find_package(Git)
find_package(fmt)
find_package(Eigen3)

# operon library
set_package_properties(Git     PROPERTIES TYPE REQUIRED)
set_package_properties(fmt     PROPERTIES TYPE REQUIRED)
set_package_properties(Eigen3  PROPERTIES TYPE REQUIRED)

FetchContent_Declare(
    gsl
    GIT_REPOSITORY https://github.com/microsoft/GSL.git
    GIT_TAG master
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/thirdparty/GSL
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/GSL
)

FetchContent_Declare(
    nanobench 
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/thirdparty/nanobench/include
    URL https://raw.githubusercontent.com/martinus/nanobench/master/src/include/nanobench.h
    DOWNLOAD_NO_EXTRACT 1
)

FetchContent_Declare(
    xxhash
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/thirdparty/xxhash
    URL https://raw.githubusercontent.com/Cyan4973/xxHash/dev/xxhash.h
    DOWNLOAD_NO_EXTRACT 1
)

FetchContent_Declare(
    robin_hood
    DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/thirdparty/robin_hood
    URL https://raw.githubusercontent.com/martinus/robin-hood-hashing/3.9.1/src/include/robin_hood.h
    DOWNLOAD_NO_EXTRACT 1
)

FetchContent_MakeAvailable(xxhash gsl nanobench robin_hood)

set(THIRDPARTY_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/thirdparty/GSL/include
    ${PROJECT_SOURCE_DIR}/thirdparty/nanobench/include
    ${PROJECT_SOURCE_DIR}/thirdparty/xxhash
    ${PROJECT_SOURCE_DIR}/thirdparty/robin_hood
    ${EIGEN3_INCLUDE_DIR}
    ${FMT_INCLUDE_DIRS}
)
message(STATUS "INCLUDE DIRS: ${THIRDPARTY_INCLUDE_DIRS}")

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_library(
    advent
    STATIC
    src/day01.cpp
    src/day02.cpp
    src/day03.cpp
    src/day04.cpp
    src/day05.cpp
    src/day06.cpp
    src/day07.cpp
    src/day08.cpp
    src/day09.cpp
    src/day10.cpp
    src/day11.cpp
    src/day12.cpp
    src/day13.cpp
    src/day14.cpp
    src/day15.cpp
    src/day16.cpp
    src/day17.cpp
    src/day18.cpp
    src/day19.cpp
    src/day20.cpp
    src/day21.cpp
    src/day22.cpp
    src/day23.cpp
    src/day24.cpp
    src/day25.cpp
)
target_compile_features(advent PRIVATE cxx_std_17)
target_include_directories(advent PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(advent SYSTEM PRIVATE ${THIRDPARTY_INCLUDE_DIRS})
target_link_libraries(advent PRIVATE fmt::fmt)
target_compile_options(advent PRIVATE "$<$<CONFIG:Debug>:-g;>$<$<CONFIG:Release>:-O3;-g;-march=znver2;-fPIC>")

add_executable(
    advent_cli
    src/advent.cpp
)
target_compile_features(advent_cli PRIVATE cxx_std_17)
target_include_directories(advent_cli PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(advent_cli SYSTEM PRIVATE ${THIRDPARTY_INCLUDE_DIRS})
target_link_libraries(advent_cli PRIVATE advent fmt::fmt)
target_compile_options(advent_cli PRIVATE "$<$<CONFIG:Debug>:-g;>$<$<CONFIG:Release>:-O3;-g;-march=znver2;-fPIC>")
