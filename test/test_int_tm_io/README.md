# Test Int TM IO

## Objective

The purpose of the test y to verify that sending and receiving telemetry is working properly.
To achieve this, a payload is created, data is loaded (emulating the data collection) and sent
to the same node, then it is checked that it has arrived correctly.

### Compiling parameters

To compile the test it's necessary to add the following compilation parameters.

>-DAPP=test_int_tm_io
>
>-DTEST=1
