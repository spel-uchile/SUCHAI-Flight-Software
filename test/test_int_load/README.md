# Test Int Load

## Objective

The purpose of the test is to send many commands and check the correct operation of the command queues.

## Compile

To compile the test type:
> cmake -B build-test -DAPP=test_int_load -DTEST=1 && cmake --build build-test

the build-test folder can be replaced by any desired name.

## Run

To run the experiment type

> ./build-test/test/test_int_load/suchai-test

the build-test folder must be the same as the one used to compile the test

## Expected results

The Command result number of all the commands called by the test must be 1, if any of those is 0,
the test will failed.

The expected console output is in the test_int_load_log_base.txt file.