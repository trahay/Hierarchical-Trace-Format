cmake_minimum_required(VERSION 3.1)

#####################################################
# trace.sh

configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/trace.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/trace.sh
  @ONLY # ask cmake to change only variables in the form @VAR@ (ie. don't touch the ${VAR} variables, which can be problematic for bash scripts)
  )

list(APPEND SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/trace.sh
  )

#####################################################
# eztrace

add_executable(read_trace
read_trace.c
)

target_compile_options(read_trace
  PRIVATE
    -Wall -Wextra -Wpedantic
)

target_include_directories(read_trace
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src/
  )

target_link_libraries(read_trace
  PRIVATE
    htf
)



install(PROGRAMS ${SCRIPTS} DESTINATION ${INSTALL_BINDIR})

install(
  TARGETS read_trace
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  RUNTIME DESTINATION ${INSTALL_BINDIR}
  INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
)
