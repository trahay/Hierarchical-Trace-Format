#!/bin/bash

CUR_PATH=$(dirname  $(realpath $0))
source "$CUR_PATH/test_utils.sh"

BUILD_DIR=$CUR_PATH

if [ $# -gt 0 ]; then
    BUILD_DIR=$1
fi


test_program="$BUILD_DIR/write_benchmark"
# First, run the test program
run_and_check_command  "${test_program}"

# Then, check that the generated trace is OK
trace_filename="${test_program}_trace/main.htf"

echo "content of ${test_program}_trace:"
ls "${test_program}_trace"

echo "content of src dir ($CUR_PATH):"
ls "${CUR_PATH}"

echo "content of build dir ($BUILD_DIR):"
ls "${BUILD_DIR}"

trace_check_integrity "$trace_filename"

# TODO:
# - check that htf_info works
# - check the parity of enter/leave events
# - check that timestamps are in correct order

