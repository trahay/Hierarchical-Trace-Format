set(dep_message "\nConfiguration of HTF:\n"
  "	General:\n"
  "		Install directory : ${CMAKE_INSTALL_PREFIX}\n"
  "		Compiler: C       : ${CMAKE_C_COMPILER} (${CMAKE_C_COMPILER_ID})\n"
  "		Operating system  : ${CMAKE_SYSTEM_NAME}\n")


set(dep_message "${dep_message}\n"
  "  Enabled modules:\n"
#  "  OMPT module    : ${EZTRACE_ENABLE_OMPT}\n"
  "  Pthread module : ${EZTRACE_ENABLE_PTHREAD}\n")

message(${dep_message})
