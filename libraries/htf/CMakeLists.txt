cmake_minimum_required(VERSION 3.14.0)

# POLICIES #####################
if (POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif ()


add_library(htf SHARED)

set(HTF_HEADERS
        include/htf/htf_archive.h
        include/htf/htf_attribute.h
        include/htf/htf_dbg.h
        include/htf/htf.h
        include/htf/htf_hash.h
        include/htf/htf_read.h
        include/htf/htf_storage.h
        include/htf/htf_timestamp.h
        include/htf/htf_write.h
        include/htf/LinkedVector.h
)

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
        PUBLIC
        ${HTF_HEADERS}
)

target_include_directories(htf
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        include
)
if (ZFP_FOUND)
    target_include_directories(htf PUBLIC ${ZFP_INCLUDE_DIRS})
    target_link_libraries(htf PRIVATE ${ZFP_LIBRARIES})
endif ()


if (SZ_FOUND)
    target_include_directories(htf PUBLIC ${SZ_INCLUDE_DIRS})
    target_link_libraries(htf PRIVATE ${SZ_LIBRARIES})
endif ()

set_property(TARGET htf
        PROPERTY PUBLIC_HEADER ${HTF_HEADERS})


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
)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

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
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDEDIR}/htf)
