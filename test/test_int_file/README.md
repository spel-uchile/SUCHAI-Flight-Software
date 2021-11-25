# Test Int File

## Objective

The purpose of the test is to send and receive files with tm_send_file, tm_senf_file_parts and tm_merge_file and 
check the received data.

### Compiling parameters

To compile the test it's necessary to add the following compilation parameters.

>-DAPP=test_int_file
>
>-DTEST=1
> 
>-DSCH_COMM_NODE=3

## Expected results

The Command result number of all the commands called by the test must be 1, if any of those is 0,
the test will failed.

The expected console output is in the test_int_file_log_base.txt file.