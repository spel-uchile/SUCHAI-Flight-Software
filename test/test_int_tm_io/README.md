# Test Int TM IO

## Objective

The purpose of the test y to verify that sending and receiving telemetry is working properly.
To achieve this, a payload is created, data is loaded (emulating the data collection) and sent
to the same node, then it is checked that it has arrived correctly.

## Compile

To compile the test type:
> cmake -B build-test -DAPP=test_int_tm_io -DTEST=1 -DSCH_ST_MODE=SQLITE - DSCH_COMM_NODE=3 && cmake --build build-test

the build-test folder can be replaced by any desired name.

## Run

To run the experiment type

> ./build-test/test/test_int_tm_io/suchai-test

the build-test folder must be the same as the one used to compile the test

## Expected results

The Command result number of all the commands called by the test must be 1, if any of those is 0,
the test will failed.

The expected console output is in the test_int_tm_io_log_base.txt file.