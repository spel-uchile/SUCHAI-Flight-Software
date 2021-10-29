# Test Int Bug Delay

## Objective

The purpose of the test is to validate the performance of osTaskDelayUntil, 
verifying that the time that the task sleeps is approximately the same as estimated.

### Compiling parameters

To compile the test it's necessary to add the following compilation parameters.

-DAPP=test_int_bug_delay
-DTEST=1
