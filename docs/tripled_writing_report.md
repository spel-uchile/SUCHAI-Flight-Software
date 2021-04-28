#Tripled writing of status variables on memory
A memory (internal or external) is needed to save the different 
system level status variables. 
With particular reference to the software, the part in charge to
set and save a value for a status variable is the 
*dat_set_system_var* method. On the other hand, the 
*dat_get_system_var* method is used to get a value
set on memory for a specific variable. Both functions are located 
in the data repository module.

##How these methods worked
####*dat_set_system_var*
This function took an index from the status variables set and a 
value to save. If an internal memory must be used, then the value 
chose was stored into *DAT_SYSTEM_VAR_BUFF* array in the position 
of the index taken. If an external memory must be used, the value
was stored into an SQLite table named *DAT_REPO_SYSTEM* in the same
way as the process to save it into an internal memory.

####*dat_get_system_var*
This method looked for the value stored on an index -indicated 
as an argument- of the memory (internal or external). 
Then, returned the value. 

##Main problem
When SUCHAI is on space, the memory will be vulnerable to known 
and -sometimes- unknown damages (there are a lot of undiscovered 
phenomenons around the space). Let's suppose that the memory
receives some kind of damage. Then, how is it possible to know
if the values saved on it are well written?

##Solution
An easy way to know if the value for the status variable written 
on memory is correct is to store replicas for the different
variables. That is, instead of writing the value on memory just 
once, two more copies of it can be stored, saving the same value
three times on different indexes. But, why three? Let's assume
a value must be got from memory: if just one copy
of the data was stored, we would not know which version to choose 
in case of both of them were different. So, the third replica act 
as a tie breaker. In the event of the three versions were unequal
from each other (worst case), there is not a right value. This will
be considered as a simple error. In addition, it is worth 
mentioning that this technique is commonly applied by distributed 
systems.

The solution given above should be developed as an option, just
in case some variable status value needs to be set/get with the
previous (and simpler) implementation. There must exist a 
constant inside the *config.h* file to enable or disable this 
option, which will be named *SCH_STORAGE_TRIPLE_WR*.

####How *dat_set_system_var* works now
In first place, as long as the *SCH_STORAGE_TRIPLE_WR* is 
enabled, the size of *DAT_SYSTEM_VAR_BUFF* (internal memory) 
needs to be three times bigger than it was before (three copies
are saved for each variable). This is stablished on the initial 
conditions for the data repository module. On the other hand, 
there is no need to worry for the external memory, it has its own 
size limit, which is big enough.

The methodology of storing a value on memory is basically the same.
The original value will be set on memory as usual, let's say on
index *i*. Then, the respective copies will be kept on index
*i + dat_status_last_address* and *dat_status_last_address * 2 + index*,
where *dat_status_last_address* is the last index on the set of status
variables that makes reference to its size. In this way, the values
will be distributed fairly into the buffer. Thus, it'll be easier 
to get them and check if they are correct.

####How *dat_get_system_var* works now
If the *SCH_STORAGE_TRIPLE_WR* is enabled, three variables are 
needed to save every copy of a value previously set on indexes 
indicated by *dat_set_system_var*. To explain this better, let's say 
the original value stored on index *i* (its respective copies are 
stored then on index *i + dat_status_last_address* and *i + 
dat_status_last_address * 2*) must be obtained: each one of 
the three variables declared will keep the value and its replicas.
Now, to confirm the memory is not totally corrupted, these three
values should be compared: at least two copies must be equal to 
each other. In the worst case, there won't be any value equal
to each other. This last thing means that memory lectures are
not reliable, thus an error message will be thrown and the return
value will be -1 (any integer could be valid with regard to status
variables -even the negative ones-, so the standard error code was 
selected).   

## Testing
The chosen alternative to test the functionality of the new
implementation was through unit testing, due to all of its known
benefits. Thus, two unit tests were implemented, one to verify
the functionality of *dat_set_system_var* -named testDATSET_SYSVAR-
and the other for *dat_get_system_var* -named testDATGET_SYSVAR-.
Both methods are included in the third suite of the main program
for unit tests folder.

**It is very important to consider that *SCH_STORAGE_TRIPLE_WR*
constant must enable the tripled writing methodology**. If this
option is off, the tests could probably fail.

It has to be mentioned that the functions *dat_set_system_var* 
and *dat_get_system_var* implemented initially were reused 
internally, that is, they are intended only for testing purposes. 
To explain this better, let's see how the tests work.  

####*testDATSET_SYSVAR* 
The idea of this test is simple: the first thing to do is 
to set a value for each status variable (exactly one random 
variable will take a random value, the rest will take a constant 
number), then they must be got from memory to confirm the 
original values and their respective copies are the same. 
To get these values, *dat_get_system_var* can't be used, because, 
in that case, the same method will be testing *dat_set_system_var*
and *dat_get_system_var* at the same time. That's why a function 
that can get single values is needed. This is the moment when the 
*dat_get_system_var* function written initially will be reused.

The final step is checking the values and their 
replicas are equal to each other. But not just that, it must be 
checked if the extracted values are the same than set values. 
Thus, there are three asserts methods for each variable, with 
each assert testing the previous description.

####*testDATGET_SYSVAR*
To test *dat_get_system_var*, the values must be set first, so
they can be obtained with this function. The *dat_set_system_var*
method initially written will be reused for this setting purpose
for the same reason as it was mentioned before (no more than
one method must be tested in a unit test). In this case, one 
random variable will be set as the worst case: its original value
and its copies will be unequal from each other. The rest of
variables will be set correctly with a random number, which
will be the same for every variable. Then, the values will be get
one by one. If the index checked is equal to the index of the
variable that was set incorrectly, the assert function value must 
be -1. In the other cases, the assert function value must be the
value set (correctly) for each variable. 

####Results
When an internal memory is used, the following results are shown:

    Test: test of dat_set_system_var ...passed


    Test: test of dat_get_system_var ...[ERROR][1536246607][repoData] Unable to get a correct value for index 10
    passed



    Run Summary:    Type  Total    Ran Passed Failed Inactive
                             suites      3      3    n/a      0        0
                               tests      6      6      6      0        0
                            asserts  114  114  114      0      n/a

    Elapsed time =    0.000 seconds
    
In the case of an external memory used, the following results are
shown:

    Test: test of dat_set_system_var ...passed
    Test: test of dat_get_system_var ...[ERROR][1536252720][repoData] Unable to get a correct value for index 1
    passed

    Run Summary:    Type  Total    Ran Passed Failed Inactive
                  suites      3      3    n/a      0        0
                   tests       6      6      6      0        0
                 asserts  114  114  114      0      n/a

    Elapsed time =    0.697 seconds

   