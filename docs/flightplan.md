# Flight Plan

## Idea Behind

The flight plan consist in a table named flightPlan.
This table has the following form:

   
   
   | time      | command   | arguments  | executions    | periodical   |
   | --------- |---------- | ---------- | ------------- | ------------ | 
   | (int)     | (string)  | (string)   | (int)         | (int)        | 
    
    
Where the attributes are:

- time : the time when the command will be executed, this time is saved in the 
table in UNIXTIME

- command : The command's name to be executed

- arguments : The arguments needed by the command to be executed

- executions : The times that the command will be executed

- periodical : The seconds where the command will be executed again

So the taskFlightPlan is checking every second if he needs to execute a command 
consulting the database.


## How to use it

### Add a command in flight plan

- Command : fp_set_cmd

- Parameters : `<day> <month> <year> <hour> <min> <sec> <command> <arguments> <executions> <periodical>`

- Function : Set a `<command>` with its `<arguments>` in a certain date and time to be executed 
`<executions>` times and will be executed again in `<periodical>` seconds

- For the next examples we assume that the time is 17:00 and the date is 26/12/2017

- Example : To set the command ping (that has the number 10 for argument) everyday at 6am 
we need to write the next line `fp_set_cmd 27 12 2017 06 00 00 ping 10 1 86400`.

- Example : To set the command help (help has not arguments) at 8pm we need to write
the next line `fp_set_cmd 26 12 2017 20 00 00 help null 1 0`.

- Example : To set the command send (that has a integer and a string for arguments) at 7pm we 
need to write the next line `fp_set_cmd 26 12 2017 19 00 00 send 10hola 1 0`.

### Delete a command in flight plan

- Command : fp_del_cmd 

- Parameters : `<day> <month> <year> <hour> <min> <sec>`

- Function : Delete the command to be executed in the time and date given in the entry

- For the next examples we assume that the table has the commands set in the section above

- Example : To delete the command ping set in the section above we need to write the 
next line `fp_del_cmd 27 12 2017 06 00 00`.

### Show the flight plan

- Command : fp_show

- Parameters : This command has not parameters

- Function : Show the flight plan table, if the flight plan table is empty just show
a message saying this.

### Reset the flight plan

- Command : fp_reset

- Parameters : This command has not parameters

- Function : Drop the table and create a new and empty table for the fight plan

