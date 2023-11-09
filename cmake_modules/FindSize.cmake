# A simple module to get the size of std::vector and std::map

set(src_content
        "@include@
#include <iostream>

int main() {
    std::cout << sizeof(@structure@);
    return 0;
}"
)

# Starting with the std::vector
set(include "#include <vector>")
set(structure "std::vector<int>")
string(CONFIGURE "${src_content}" vector_src @ONLY)
try_compile(HAVE_VECTOR
        SOURCE_FROM_VAR vectorSize.cpp vector_src
        COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/vectorSize.bin)

if (HAVE_VECTOR)
    execute_process(
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vectorSize.bin
            OUTPUT_VARIABLE VECTOR_SIZE_BIN_OUTPUT
            RESULT_VARIABLE VECTOR_SIZE_BIN_RESULT
    )
    message("sizeof(std::vector) = ${VECTOR_SIZE_BIN_OUTPUT}")
    add_compile_definitions("VECTOR_SIZE=${VECTOR_SIZE_BIN_OUTPUT}")
else ()
    message("Couldn't compile code with vectors.")
endif ()

# Then with the std::map
set(include "#include <map>")
set(structure "std::map<int, int>")
string(CONFIGURE "${src_content}" map_src @ONLY)
try_compile(HAVE_MAP
        SOURCE_FROM_VAR mapSize.cpp map_src
        COPY_FILE ${CMAKE_CURRENT_BINARY_DIR}/mapSize.bin)

if (HAVE_MAP)
    execute_process(
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mapSize.bin
            OUTPUT_VARIABLE MAP_SIZE_BIN_OUTPUT
            RESULT_VARIABLE MAP_SIZE_BIN_RESULT
    )
    message("sizeof(std::map) = ${MAP_SIZE_BIN_OUTPUT}")
    add_compile_definitions("MAP_SIZE=${MAP_SIZE_BIN_OUTPUT}")
else ()
    message("Couldn't compile code with maps.")
endif ()