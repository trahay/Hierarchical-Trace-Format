cmake_minimum_required(VERSION 3.13)

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories(${CMAKE_SOURCE_DIR}/src/)
link_libraries(htf zstd)

add_executable(htf_print htf_print.c )
add_executable(htf_info htf_info.c )

install(
  TARGETS htf_print htf_info
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  RUNTIME DESTINATION ${INSTALL_BINDIR}
  INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
)
