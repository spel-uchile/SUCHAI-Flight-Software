# Test Int Bug Delay

## Objective

The purpose of the test is to validate the performance of osTaskDelayUntil, 
verifying that the time that the task sleeps is approximately the same as estimated.

## Compiling parameters

To compile the test it's necessary to add the following compilation parameters.

>-DAPP=test_int_bug_delay
>
>-DTEST=1
 
## Expected results

If the test fails, this message will appeared in the terminal and the program will be aborted

>taskTest: Assertion `xFinalTime - xStartTime >= 0.999*TEST_MAX_SECONDS*1000000.0' failed.

If the program ends correctly, the test will pass.
