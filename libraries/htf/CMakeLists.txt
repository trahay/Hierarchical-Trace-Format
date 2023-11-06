cmake_minimum_required(VERSION 3.14.0)

# POLICIES #####################
if (POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif ()


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
        src/ParameterHandler.cpp
        src/ParameterHandler.h
)

target_include_directories(htf
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        include
        ${zfp_INCLUDE_DIRS}
        ${sz_INCLUDE_DIRS}
)


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
        ${JSONCPP_LIBRARIES}
        ${zfp_LIBRARIES}
        ${sz_LIBRARIES}
)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(htf_doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

install(TARGETS htf EXPORT HTFLibTargets
        LIBRARY DESTINATION ${INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR})
