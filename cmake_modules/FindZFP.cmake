# - Find zstd
# Find the zstd compression library and includes
#
# ZFP_INCLUDE_DIRS - where to find ZFP.h, etc.
# ZFP_LIBRARIES - List of libraries when using ZFP.
# ZFP_FOUND - True if ZFP found.

find_path(ZFP_INCLUDE_DIRS
        NAMES zfp.h
        HINTS ${ZFP_ROOT_DIR}/include)

find_library(ZFP_LIBRARIES
        NAMES zfp
        HINTS ${ZFP_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZFP DEFAULT_MSG ZFP_LIBRARIES ZFP_INCLUDE_DIRS)

mark_as_advanced(
        ZFP_LIBRARIES
        ZFP_INCLUDE_DIRS)

if (ZFP_FOUND AND NOT (TARGET ZFP::ZFP))
    add_library(ZFP::ZFP UNKNOWN IMPORTED)
    set_target_properties(ZFP::ZFP
            PROPERTIES
            IMPORTED_LOCATION ${ZFP_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${ZFP_INCLUDE_DIRS})
endif ()