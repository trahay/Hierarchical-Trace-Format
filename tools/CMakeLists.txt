cmake_minimum_required(VERSION 3.1)

#####################################################
# htf_print

add_executable(htf_print
htf_print.c
)

target_compile_options(htf_print
  PRIVATE
    -Wall -Wextra -Wpedantic
)

target_include_directories(htf_print
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src/
  )

target_link_libraries(htf_print
  PRIVATE
    htf
)

install(
  TARGETS htf_print
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  RUNTIME DESTINATION ${INSTALL_BINDIR}
  INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
)
