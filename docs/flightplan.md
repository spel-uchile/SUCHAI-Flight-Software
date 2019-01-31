# Flight Plan

## Format and Concept

The flight plan module consists of a table named flightPlan.
This table follows the following standard:

   | `time` | `command` | `arguments` | `executions` | `periodical` |
   | ------ |---------- | ----------- | ------------ | ------------ | 
   | (int)  | (string)  | (string)    | (int)        | (int)        | 

Where the attributes are:

- `time` : The time value of when the command will be first executed, this time is saved in the 
table as UNIX TIME.

- `command` : The name of the command to be executed.

- `arguments` : The arguments needed by the command to be executed, separated by spaces.

- `executions` : The amount of times the command will be executed back to back.

- `periodical` : If the command should execute periodically, this value is the unix time period for that
cycle. If the command isn't meant to execute multiple times, this value is 0.

In LINUX database implementations of the flight plan storage, the`time` column serves as the primary 
key for the table.

The taskFlightPlan module checks the flight plan table periodically to see if a command can be executed.

Commands only execute when the system time and their `time` values match.

## Caveats and Details

Keep in mind that a command with a `executions` value of 10 and a `periodical` value of 1000 will 
execute 10 times very 1000 seconds until it's removed by another command, or left obsolete.
 
A command can be left obsolete either when:

- Another command with the same `time` value overwrites it's place in the table (In database 
implementations), resulting on the deletion of the obsolete command.
 
- A command that takes at least a second to execute completely and has the same `time` value is executed 
first, leaving the obsolete command as trash information in the table forever.

- A command that takes `S` seconds to execute and has a `time` value of at most `S-1` seconds less 
executes, leaving no time for `taskFlightPlan` to find the obsolete command and keeping 
it as trash information forever as well.

## Usage instructions

#### Adding a command to the flight plan

- Command : `fp_set_cmd`

- Parameters : `<day> <month> <year> <hour> <min> <sec> <command> <arguments> <executions> <periodical>`

- Function : Set a `<command>` with its `<arguments>` in a certain date and time to be executed 
`<executions>` times every `<periodical>` seconds, or only `<executions>` times if `<periodical>` is 0.

- If the command to set requires no arguments, `<arguments>` can be the string `null`.

- If the command to set requires more han one argument, `<arguments>` is the string of them 
separated by commas instead of spaces.

For the next examples we assume that the time is 17:00 and the date is 26/12/2017 (December 26th, 2017)

- Example : To set the command ping (that has the number 10 for argument) everyday at 6am 
we need to write the line `fp_set_cmd 27 12 2017 06 00 00 ping 10 1 86400`

- Example : To set the command help (help has not arguments) at 8pm we need to write
the line `fp_set_cmd 26 12 2017 20 00 00 help null 1 0`

- Example : To set the command send (that has a integer and a string for arguments) at 7pm we 
need to write the line `fp_set_cmd 26 12 2017 19 00 00 send 10,hola 1 0`

#### Deleting a command from the flight plan

- Command : `fp_del_cmd`

- Parameters : `<day> <month> <year> <hour> <min> <sec>`

- Function : Delete the command to be executed in the given time and date.

For the next examples we assume that the table has the commands set in the above section

- Example : To delete the ping command ping from the table we need to write the 
line `fp_del_cmd 27 12 2017 06 00 00`

#### Printing the flight plan table

- Command : `fp_show`

- Parameters : This command has not parameters

- Function : Print the flight plan table, if the flight plan table is empty it prints nothing 
(A `Flight plan table empty` message prints to the `INFO` log).

#### Resetting the flight plan

- Command : `fp_reset`

- Parameters : This command has not parameters

- Function : Drops the table and creates a new and empty table for the fight plan.

