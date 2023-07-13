cmake_minimum_required(VERSION 3.1)

# POLICIES #####################
if (POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()


add_library(htf SHARED)

set(HTF_HEADERS
        htf.h
        htf_read.h
        htf_write.h
        htf_archive.h
        htf_dbg.h
        htf_timestamp.h
        htf_storage.h
        htf_vector.h
        htf_hash.h
        )

target_sources(
        htf
        PRIVATE
        htf_read.c
        htf_write.c
        htf_archive.c
        htf_timestamp.c
        htf_storage.c
        htf_dbg.c
        htf.c
        htf_vector.c
        htf_hash.c
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
        zstd
        PUBLIC
    dl
    PRIVATE
    ${CMAKE_DL_LIBS}
    rt
    m
    ${zstd_LIBRARIES}
)
install(TARGETS htf EXPORT HTFLibTargets
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR})
