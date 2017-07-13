cmake_minimum_required(VERSION 3.2)

project(ga CXX)

message (STATUS "C++ compiler is ${CMAKE_CXX_COMPILER}")

if (NOT MSVC)
    add_compile_options(-std=c++14)
endif()

add_library(ga  src/include/ga.hpp
                src/ga.cpp
                src/include/detail/detail.hpp
                src/include/random_generator.hpp
                src/include/genotype_model.hpp
                src/include/genotype_constructor.hpp
                src/include/operators/mutation.hpp
                src/include/operators/crossover.hpp
                src/include/population.hpp src/include/functions.hpp src/include/logger.hpp)

target_include_directories(ga PUBLIC src/include)

add_executable(example_optimisation src/examples/optimisation.cpp)
#target_link_libraries(example_optimisation PRIVATE ga)

if (WITH_TESTS)
    add_library(test_lib src/tests/test_lib/include/test.hpp
                         src/tests/test_lib/test.cpp)

    target_include_directories(test_lib PUBLIC src/tests/test_lib/include)

    add_executable(ga_test src/tests/test.cpp)
    target_link_libraries(ga_test PRIVATE test_lib)
endif()