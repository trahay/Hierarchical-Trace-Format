cmake_minimum_required(VERSION 3.1)

# POLICIES #####################
if (POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()


add_library(htf
  SHARED
)

set(HTF_HEADERS
    event.h
    timestamp.h
    trace_storage.h
)

target_sources(
    htf
PRIVATE
    timestamp.c
    trace_storage.c
    write_events.c
PUBLIC
    ${HTF_HEADERS}
)

set_property(TARGET htf
 PROPERTY PUBLIC_HEADER ${HTF_HEADERS})

target_compile_options(htf
  PUBLIC
    -D_GNU_SOURCE
  PRIVATE
    -Wall -Wextra
)

target_link_libraries(htf
  PRIVATE
    atomic
  PUBLIC
    dl
    PRIVATE
    ${CMAKE_DL_LIBS}
    rt
    m
    )

install(TARGETS htf EXPORT HTFLibTargets
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR})
