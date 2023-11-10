# - Find zstd
# Find the zstd compression library and includes
#
# SZ_INCLUDE_DIRS - where to find SZ.h, etc.
# SZ_LIBRARIES - List of libraries when using SZ.
# SZ_FOUND - True if SZ found.

find_path(SZ_INCLUDE_DIRS
        NAMES sz.h
        HINTS ${SZ_ROOT_DIR}/include/sz)

find_library(SZ_LIBRARIES
        NAMES SZ
        HINTS ${SZ_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SZ DEFAULT_MSG SZ_LIBRARIES SZ_INCLUDE_DIRS)

mark_as_advanced(
        SZ_LIBRARIES
        SZ_INCLUDE_DIRS)

if (SZ_FOUND AND NOT (TARGET SZ::SZ))
    add_library(SZ::SZ UNKNOWN IMPORTED)
    set_target_properties(SZ::SZ
            PROPERTIES
            IMPORTED_LOCATION ${SZ_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${SZ_INCLUDE_DIRS})
endif ()