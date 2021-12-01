# Test Int File

## Objective

The purpose of the test is to send and receive files with tm_send_file, tm_send_file_parts and tm_merge_file and 
check the received data.

## Compile

To compile the test type:
> cmake -B build-test -DAPP=test_int_file -DTEST=1 -DSCH_COMM_NODE=3 && cmake --build build-test

the build-test folder can be replaced by any desired name.

## Run

To run the experiment type

> ./build-test/test/test_int_file/suchai-test

the build-test folder must be the same as the one used to compile the test

## Expected results

The Command result number of all the commands called by the test must be 1, if any of those is 0,
the test will failed.

The expected console output is in the test_int_file_log_base.txt file.