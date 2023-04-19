cmake_minimum_required (VERSION 3.1)
project(HTF
  VERSION 0.1.0
  LANGUAGES C
)

# include CMake modules
include(CheckLibraryExists)
include(CheckLanguage)
include(CTest)
include(GNUInstallDirs)

# RPATH 
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_SKIP_BUILD_RPATH FALSE) # use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # when building, don't use the install RPATH already (but later on when installing)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}") # the RPATH to be used when installing

# CMakeFind repository
list (APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)


# Look for libraries

# Options to enable/disable modules
option(ENABLE_PTHREAD  "Enable Pthread module"  ON)
option(ENABLE_MPI      "Enable MPI module"  OFF)
#option(ENABLE_OMPT     "Enable OMPT module"     OFF)

option(ENABLE_OTF2     "Enable OTF2 compatibilit layer"     ON)


if (ENABLE_MPI)
  find_program(MPICC mpicc REQUIRED)
endif()

####################################################
# Some flags for compilation

set (prefix "${CMAKE_INSTALL_PREFIX}")
set(LIBRARY_PATH "${CMAKE_INSTALL_PREFIX}/lib")
if ("${APPLE}")
  set(DYNLIB_EXT "dylib")
  set(LD_PRELOAD_NAME "DYLD_INSERT_LIBRARIES")
  set(LD_LIBRARY_PATH_NAME "DYLD_LIBRARY_PATH")
else() # By default -> Linux
  set(DYNLIB_EXT "so")
  set(LD_PRELOAD_NAME "LD_PRELOAD")
  set(LD_LIBRARY_PATH_NAME "LD_LIBRARY_PATH")
endif()

set(HTF_LIB_DIR ${CMAKE_INSTALL_FULL_LIBDIR})

set(INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set(INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
set(INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(INSTALL_PKGCONFIG_DIR "${INSTALL_LIBDIR}/pkgconfig")

# Subdirectory
add_subdirectory (src)
if(ENABLE_OTF2)
add_subdirectory (src/otf2)
endif()
add_subdirectory (tracer)
add_subdirectory (tools)
add_subdirectory (test)
if (ENABLE_MPI)
  add_subdirectory (test/mpi)
endif()

include(PrintOpts)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/htf.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/htf.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/htf.pc DESTINATION "${INSTALL_PKGCONFIG_DIR}")
