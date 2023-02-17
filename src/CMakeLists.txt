cmake_minimum_required(VERSION 3.1)

# POLICIES #####################
if (POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()


add_library(htf
  SHARED
)

set(HTF_HEADERS
    htf.h
    htf_timestamp.h
    htf_storage.h
)

target_sources(
    htf
PRIVATE
    htf_timestamp.c
    htf_storage.c
    htf.c
PUBLIC
    ${HTF_HEADERS}
)

set_property(TARGET htf
 PROPERTY PUBLIC_HEADER ${HTF_HEADERS})

target_compile_options(htf
  PUBLIC
    -D_GNU_SOURCE
  PRIVATE
    -Wall -Wextra -Wno-sign-compare
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
