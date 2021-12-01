# Test Int Bug Delay

## Objective

The purpose of the test is to validate the performance of osTaskDelayUntil, 
verifying that the time that the task sleeps is approximately the same as estimated.

## Compile

To compile the test type:
> cmake -B build-test -DAPP=test_int_bug_delay -DTEST=1 && cmake --build build-test

the build-test folder can be replaced by any desired name.

## Run

To run the experiment type

> ./build-test/test/test_int_bug_delay/suchai-test

the build-test folder must be the same as the one used to compile the test

## Expected results

If the test fails, this message will appeared in the terminal and the program will be aborted

>taskTest: Assertion `xFinalTime - xStartTime >= 0.999*TEST_MAX_SECONDS*1000000.0' failed.

If the program ends correctly, the test will pass.
