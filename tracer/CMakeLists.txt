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
# htf-trace

add_library(htf-trace
  SHARED
)

target_sources(
    htf-trace
PRIVATE
    tracer.c
    cond.c
    mutex.c
)

target_include_directories(htf-trace
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src/
  )

target_link_libraries(htf-trace
  PRIVATE
    htf
)

install(PROGRAMS ${SCRIPTS} DESTINATION ${INSTALL_BINDIR})

install(
  TARGETS htf-trace
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
)
