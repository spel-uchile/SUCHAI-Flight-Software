# Test Unit

## Objective

The purpose of the test is:
- check that the commands are processed correctly, tests are made for correct 
and invalid commands and with or without arguments.
- check commands related to the flight plan, the data repository, the quaternions 
library and matrices usage.

## Compiling parameters

To compile the test it's necessary to add the following compilation parameters.

>-DAPP=test_unit
>
>-DTEST=1
> 
>-DSCH_STORAGE_TRIPLE_WR=1

It's also necessary to install CUnit library, use:

> sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev

## Expected results

The following information should appear in the terminal:

![Test Unit result](test_unit_result.png)

It's necessary to ensure that all test passed.