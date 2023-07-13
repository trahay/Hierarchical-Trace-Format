cmake_minimum_required(VERSION 3.1)

enable_testing()

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories(${CMAKE_SOURCE_DIR}/src/)
link_libraries(htf)

#####################################################
# write_benchmark

add_executable(write_benchmark write_benchmark.c)
add_test(NAME write_benchmark COMMAND write_benchmark)

add_executable(find_loop find_loop.c)
add_test(NAME find_loop COMMAND find_loop)

add_executable(test_vector test_vector.c)
add_test(NAME test_vector COMMAND test_vector)

add_executable(test_hash test_hash.c)
add_test(NAME test_hash COMMAND test_hash)
