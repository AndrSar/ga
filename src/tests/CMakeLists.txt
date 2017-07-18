add_library(test_lib test_lib/include/test.hpp
                     test_lib/test.cpp)

target_include_directories(test_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/test_lib/include)

add_executable(ga_test test.cpp)
target_link_libraries(ga_test PRIVATE test_lib)

add_test(NAME cmake_ga_test COMMAND ga_test)