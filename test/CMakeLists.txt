cmake_minimum_required(VERSION 3.13)

enable_testing()
# add_subdirectory(mpi)

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories(${CMAKE_SOURCE_DIR}/src/)
link_libraries(htf)
configure_file(../config.json config.json COPYONLY)

#####################################################
# write_benchmark

add_executable(write_benchmark write_benchmark.c)
add_test(build_write_benchmark "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target write_benchmark)
add_test (write_benchmark_tests bash "${CMAKE_CURRENT_SOURCE_DIR}/write_benchmark.sh" "${CMAKE_CURRENT_BINARY_DIR}" DEPENDS build_write_benchmark)



add_executable(find_loop find_loop.c)
add_test(NAME find_loop COMMAND find_loop 50 100)

add_executable(test_vector test_vector.c)
add_test(NAME test_vector COMMAND test_vector 10 100)

add_executable(test_hash test_hash.cpp)
#add_test(NAME test_hash COMMAND test_hash)
