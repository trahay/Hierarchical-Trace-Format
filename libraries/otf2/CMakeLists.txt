cmake_minimum_required(VERSION 3.14.0)

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

add_executable(otf2-print src/otf2-print.c)

target_compile_options(otf2-print
  PRIVATE
    -Wall -Wextra -Wpedantic
)

target_link_libraries(otf2-print
  PRIVATE
    htf
    otf2
)



#####################################################
# libotf2

add_library(otf2 SHARED)

target_sources(
    otf2
PRIVATE
        src/otf2.c
        src/OTF2_Archive.c
        src/OTF2_AttributeList.c
        src/OTF2_AttributeValue.c
        src/OTF2_DefReaderCallbacks.c
        src/OTF2_DefReader.c
        src/OTF2_DefWriter.c
        src/OTF2_ErrorCodes.c
        src/OTF2_EventSizeEstimator.c
        src/OTF2_EvtReaderCallbacks.c
        src/OTF2_EvtReader.c
        src/OTF2_EvtWriter.c
        src/OTF2_GlobalDefReaderCallbacks.c
        src/OTF2_GlobalDefReader.c
        src/OTF2_GlobalDefWriter.c
        src/OTF2_GlobalEvtReaderCallbacks.c
        src/OTF2_GlobalEvtReader.c
        src/OTF2_GlobalSnapReaderCallbacks.c
        src/OTF2_GlobalSnapReader.c
        src/OTF2_IdMap.c
        src/OTF2_MarkerReaderCallbacks.c
        src/OTF2_MarkerReader.c
        src/OTF2_MarkerWriter.c
        src/OTF2_Reader.c
        src/OTF2_SnapReaderCallbacks.c
        src/OTF2_SnapReader.c
        src/OTF2_SnapWriter.c
        src/OTF2_Thumbnail.c
)

target_compile_options(otf2
  PUBLIC
    -D_GNU_SOURCE
  PRIVATE
-Wno-unused-parameter
-Wall -Wextra -Wno-sign-compare
)

target_include_directories(otf2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} include)

target_link_libraries(otf2 PRIVATE htf)

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
