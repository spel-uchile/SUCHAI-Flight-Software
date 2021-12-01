# Test Int CMD

## Objective

EThe purpose of the test is to check several commonly used FSS commands. First we check that the arguments 
of the commands are parsed correctly, and finally the OBC commands are tested.

To check for correctness we see that all asserts pass and that all commands return 1.

## Compile

To compile the test type:
> cmake -B build-test -DAPP=test_int_cmd -DTEST=1 && cmake --build build-test

the build-test folder can be replaced by any desired name.

## Run

To run the experiment type

> ./build-test/test/test_int_cmd/suchai-test

the build-test folder must be the same as the one used to compile the test

## Expected results

The Command result number of all the commands called by the test must be 1, if any of those is 0, 
the test will failed.

The expected console output is in the test_int_cmd_log_base.txt file.