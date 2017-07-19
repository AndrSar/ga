
#          Copyright Andrey Lifanov 2017.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.2)

cmake_policy(SET CMP0048 NEW)

project(ga VERSION 0.1.0 LANGUAGES CXX)

message (STATUS "C++ compiler is ${CMAKE_CXX_COMPILER}")

if (NOT MSVC)
    add_compile_options(-std=c++14)
endif()

add_library(ga INTERFACE)
target_sources(ga INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ga.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/api.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/detail/detail.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/random_generator.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/genotype_model.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/genotype_constructor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/operators/mutation.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/operators/crossover.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/population.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/functions.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/logging/logger.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/statistics.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/logging/console_logger.hpp)

target_include_directories(ga INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/include)


if (WITH_EXAMPLES)
    message (STATUS "Including examples")
    add_subdirectory(src/examples)
endif()


if (WITH_TESTS)
    enable_testing()
    message (STATUS "Including tests")
    add_subdirectory(src/tests)
endif()


# Installation

install(DIRECTORY src/include/ DESTINATION include/ga)