cmake_minimum_required(VERSION 3.1)

enable_testing()


#####################################################
# write_benchmark

add_executable(write_benchmark
  write_benchmark.c
)

target_compile_options(write_benchmark
  PRIVATE
    -Wall -Wextra -Wpedantic
)

target_include_directories(write_benchmark
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src/
  )

target_link_libraries(write_benchmark
  PRIVATE
    htf
)

add_test(NAME write_benchmark COMMAND write_benchmark)
