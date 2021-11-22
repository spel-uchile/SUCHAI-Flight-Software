# Test Int Load

## Objective

The purpose of the test is to send many commands and check the correct operation of the command queues.

### Compiling parameters

To compile the test it's necessary to add the following compilation parameters.

>-DAPP=test_int_load
>
>-DTEST=1

## Expected results

The Command result number of all the commands called by the test must be 1, if any of those is 0,
the test will failed.

The expected console output is in the test_int_load_log_base.txt file.