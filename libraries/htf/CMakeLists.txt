cmake_minimum_required(VERSION 3.14.0)

# POLICIES #####################
if (POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()


add_library(htf SHARED)

target_sources(htf
        PRIVATE
        src/htf.cpp
        src/htf_archive.cpp
        src/htf_attribute.cpp
        src/htf_dbg.cpp
        src/htf_hash.cpp
        src/htf_read.cpp
        src/htf_storage.cpp
        src/htf_timestamp.cpp
        src/htf_write.cpp
        src/LinkedVector.cpp
)

target_include_directories(htf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} include)


target_compile_options(htf
  PUBLIC
    -D_GNU_SOURCE
  PRIVATE
    -Wall -Wextra -Wno-sign-compare
)


target_link_libraries(htf
        PUBLIC
        dl
        PRIVATE
        atomic
        ${CMAKE_DL_LIBS}
        rt
        m
        zstd
        ${zstd_LIBRARIES}
)
install(TARGETS htf EXPORT HTFLibTargets
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR})
