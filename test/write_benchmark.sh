#!/bin/bash

CUR_PATH=$(dirname  $(realpath $0))
source "$CUR_PATH/test_utils.sh"

BUILD_DIR=$CUR_PATH

if [ $# -gt 0 ]; then
    BUILD_DIR=$1
fi

nb_failed=0
nb_pass=0

test_program="write_benchmark"

# First, run the test program

niter=20
nthread=4

# we need to move to the build dir to make sure the trace is not generated in the source directory
cd "$BUILD_DIR"
run_and_check_command  "./${test_program}"  -n $niter -t $nthread
# Then, check that the generated trace is OK
trace_filename="${test_program}_trace/main.htf"

trace_check_existence "$trace_filename"
trace_check_htf_print "$trace_filename"

# TODO:
# - check that htf_info works:
#trace_check_htf_info "$trace_filename"

trace_check_enter_leave_parity "$trace_filename"

trace_check_nb_function "$trace_filename" function_0 $(expr $niter \* $nthread)
trace_check_nb_function "$trace_filename" function_1 $(expr $niter \* $nthread)



# Run the benchmark again with a logical clock
run_and_check_command  "./${test_program}"  -n $niter -t $nthread -l

# Then, check that the generated trace is OK
trace_filename="${test_program}_trace/main.htf"

trace_check_existence "$trace_filename"
trace_check_htf_print "$trace_filename"

# TODO:
# - check that htf_info works:
#trace_check_htf_info "$trace_filename"

trace_check_enter_leave_parity "$trace_filename"

trace_check_nb_function "$trace_filename" function_0 $(expr $niter \* $nthread)
trace_check_nb_function "$trace_filename" function_1 $(expr $niter \* $nthread)

trace_check_timestamp_order "$trace_filename" thread_0
trace_check_timestamp_order "$trace_filename" thread_1
trace_check_timestamp_order "$trace_filename" thread_2
trace_check_timestamp_order "$trace_filename" thread_3

echo "results: $nb_pass pass, $nb_failed failed"
if [ $nb_failed -gt 0 ]; then
    exit 1;
else
    exit 0;
fi
