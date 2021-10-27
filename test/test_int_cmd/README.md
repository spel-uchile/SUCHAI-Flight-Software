# Test Int CMD

## Objective

EThe purpose of the test is to check several commonly used FSS commands. First we check that the arguments 
of the commands are parsed correctly, and finally the OBC commands are tested.

To check for correctness we see that all asserts pass and that all commands return 1.

### Compiling parameters

To compile the test it's necessary to add the following compilation parameters.

-DAPP=test_int_cmd
-DTEST=1
