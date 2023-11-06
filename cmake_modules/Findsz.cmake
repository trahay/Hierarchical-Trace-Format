# - Find zstd
# Find the zstd compression library and includes
#
# sz_INCLUDE_DIRS - where to find sz.h, etc.
# sz_LIBRARIES - List of libraries when using sz.
# sz_FOUND - True if sz found.

find_path(sz_INCLUDE_DIRS
        NAMES sz.h
        HINTS ${sz_ROOT_DIR}/include/sz)

find_library(sz_LIBRARIES
        NAMES SZ
        HINTS ${sz_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(sz DEFAULT_MSG sz_LIBRARIES sz_INCLUDE_DIRS)

mark_as_advanced(
        sz_LIBRARIES
        sz_INCLUDE_DIRS)

if (sz_FOUND AND NOT (TARGET sz::sz))
    add_library(sz::sz UNKNOWN IMPORTED)
    set_target_properties(sz::sz
            PROPERTIES
            IMPORTED_LOCATION ${sz_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${sz_INCLUDE_DIRS})
endif ()