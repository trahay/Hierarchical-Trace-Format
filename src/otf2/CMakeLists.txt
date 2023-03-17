cmake_minimum_required(VERSION 3.1)

# POLICIES #####################
if (POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

#####################################################
# otf2-config

configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/otf2_config.in
  ${CMAKE_CURRENT_BINARY_DIR}/otf2-config
  @ONLY # ask cmake to change only variables in the form @VAR@ (ie. don't touch the ${VAR} variables, which can be problematic for bash scripts)
  )

list(APPEND SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/otf2-config
  )

#####################################################
# otf2-print

add_executable(otf2-print
otf2-print.c
)

target_compile_options(otf2-print
  PRIVATE
    -Wall -Wextra -Wpedantic
)

target_include_directories(otf2-print
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src/
  )

target_link_libraries(otf2-print
  PRIVATE
    htf
    otf2
)



#####################################################
# libotf2

add_library(otf2
  SHARED
)

set(OTF2_HEADERS
    OTF2_Archive.h
    OTF2_AttributeList.h
    OTF2_AttributeValue.h
    OTF2_Callbacks.h
    otf2_compiler.h
    OTF2_Definitions.h
    OTF2_DefReaderCallbacks.h
    OTF2_DefReader.h
    OTF2_DefWriter.h
    OTF2_ErrorCodes.h
    OTF2_Events.h
    OTF2_EventSizeEstimator.h
    OTF2_EvtReaderCallbacks.h
    OTF2_EvtReader.h
    OTF2_EvtWriter.h
    OTF2_GeneralDefinitions.h
    OTF2_GlobalDefReaderCallbacks.h
    OTF2_GlobalDefReader.h
    OTF2_GlobalDefWriter.h
    OTF2_GlobalEvtReaderCallbacks.h
    OTF2_GlobalEvtReader.h
    OTF2_GlobalSnapReaderCallbacks.h
    OTF2_GlobalSnapReader.h
    otf2.h
    OTF2_IdMap.h
    OTF2_Marker.h
    OTF2_MarkerReaderCallbacks.h
    OTF2_MarkerReader.h
    OTF2_MarkerWriter.h
    OTF2_MPI_Collectives.h
    OTF2_OpenMP_Locks.h
    OTF2_Pthread_Locks.h
    OTF2_Reader.h
    OTF2_SnapReaderCallbacks.h
    OTF2_SnapReader.h
    OTF2_SnapWriter.h
    OTF2_Thumbnail.h
)

target_sources(
    otf2
PRIVATE
    otf2.c
    OTF2_Archive.c
    OTF2_AttributeList.c
    OTF2_AttributeValue.c
    OTF2_DefReaderCallbacks.c
    OTF2_DefReader.c
    OTF2_DefWriter.c
    OTF2_ErrorCodes.c
    OTF2_EventSizeEstimator.c
    OTF2_EvtReaderCallbacks.c
    OTF2_EvtReader.c
    OTF2_EvtWriter.c
    OTF2_GlobalDefReaderCallbacks.c
    OTF2_GlobalDefReader.c
    OTF2_GlobalDefWriter.c
    OTF2_GlobalEvtReaderCallbacks.c
    OTF2_GlobalEvtReader.c
    OTF2_GlobalSnapReaderCallbacks.c
    OTF2_GlobalSnapReader.c
    OTF2_IdMap.c
    OTF2_MarkerReaderCallbacks.c
    OTF2_MarkerReader.c
    OTF2_MarkerWriter.c
    OTF2_Reader.c
    OTF2_SnapReaderCallbacks.c
    OTF2_SnapReader.c
    OTF2_SnapWriter.c
    OTF2_Thumbnail.c
PUBLIC
    ${OTF2_HEADERS}
)

set_property(TARGET otf2
 PROPERTY PUBLIC_HEADER ${OTF2_HEADERS})

target_compile_options(otf2
  PUBLIC
    -D_GNU_SOURCE
  PRIVATE
-Wno-unused-parameter
-Wall -Wextra -Wno-sign-compare
)

target_include_directories(otf2
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src/
  )
  
target_link_libraries(otf2
    PRIVATE
    htf
    )

install(TARGETS otf2 EXPORT OTF2LibTargets
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR}/otf2)

install(
  TARGETS otf2-print
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  RUNTIME DESTINATION ${INSTALL_BINDIR}
  INCLUDES DESTINATION ${INSTALL_INCLUDEDIR}
  )

install(PROGRAMS ${SCRIPTS} DESTINATION ${INSTALL_BINDIR})
