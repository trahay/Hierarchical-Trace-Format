cmake_minimum_required(VERSION 3.1)

enable_testing()



#if (ENABLE_MPI)

  set(CMAKE_C_COMPILER ${MPICC})

  add_compile_options(-Wall -Wextra -Wpedantic)	
  include_directories(${CMAKE_SOURCE_DIR}/src/)
  link_libraries(htf)

  add_executable(mpi_benchmark  mpi_benchmark.c)
  add_test(NAME mpi_benchmark COMMAND mpirun -np 2  ./mpi_benchmark)


#endif()
