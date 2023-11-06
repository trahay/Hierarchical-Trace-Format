# - Find zstd
# Find the zstd compression library and includes
#
# zfp_INCLUDE_DIRS - where to find zfp.h, etc.
# zfp_LIBRARIES - List of libraries when using zfp.
# zfp_FOUND - True if zfp found.

find_path(zfp_INCLUDE_DIRS
        NAMES zfp.h
        HINTS ${zfp_ROOT_DIR}/include)

find_library(zfp_LIBRARIES
        NAMES zfp
        HINTS ${zfp_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(zfp DEFAULT_MSG zfp_LIBRARIES zfp_INCLUDE_DIRS)

mark_as_advanced(
        zfp_LIBRARIES
        zfp_INCLUDE_DIRS)

if (zfp_FOUND AND NOT (TARGET zfp::zfp))
    add_library(zfp::zfp UNKNOWN IMPORTED)
    set_target_properties(zfp::zfp
            PROPERTIES
            IMPORTED_LOCATION ${zfp_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${zfp_INCLUDE_DIRS})
endif ()