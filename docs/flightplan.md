# Flight Plan

## Format and Concept

The flight plan module consists of a table named flightPlan.
This table follows the following standard:

   | `time` | `executions` | `periodical` | `command` | `arguments` |
   | ------ |------------- | ------------ | --------- | ----------- |
   | (int)  | (int)        | (int)        | (string)  | (string)    |

Where the attributes are:

- `time`: The time value of when the command will be first executed, this time is saved in the 
table as UNIX TIME.
- `periodical`: If the command should execute periodically, this value is the unix time period for that
- `executions`: The amount of times the command will be executed back to back.
- `command`: The name of the command to be executed.
- `arguments`: The arguments needed by the command to be executed, separated by spaces.
cycle. If the command isn't meant to execute multiple times, this value is 0.

In LINUX database implementations of the flight plan storage, the`time` column serves as the primary 
key for the table.

The `taskFlightPlan` module checks the flight plan table periodically (1 second) to see if a command can be executed.

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

## Implementation Details

### Flash Memory Storage in the NANOMIND architecture

The flight plan is saved using the following scheme of bits:

- `timetodo(uint32_t)`
- `executions(uint32_t)` 
- `periodical(uint32_t)` 
- `name_length(uint32_t)` 
- `args_length(uint32_t)` 
- `name(char*SCH_CMD_MAX_STR_NAME)` 
- `args(char*SCH_CMD_MAX_STR_PARAMS)`

In that order.

`SCH_CMD_MAX_STR_NAME` is the maximum allowed length of a command's name and `SCH_CMD_MAX_STR_PARAMS` is the
maximum allowed length of a parameters string.

Where `timetodo` is saved on it's own, `name` and `args` are saved as concatenations of chars, and
`executions`, `periodical`, `name_length` and `args_length` are saved copying the memory used by 
the `struct`:
```{c}
typedef struct {
    uint32_t exec, peri, name_len, args_len;
} numbers_container_t;
```
And therefore respect it's bit alignment.

Due to this, the byte size of a command in flash storage is given by the expression:
```{c}
static int max_command_size = sizeof(uint32_t)+sizeof(numbers_container_t)+sizeof(char)*(SCH_CMD_MAX_STR_NAME+SCH_CMD_MAX_STR_PARAMS);
```

## Usage instructions

#### Adding a command to the flight plan

- Command : `fp_set_cmd`
  - Parameters : `<day> <month> <year> <hour> <min> <sec> <executions> <periodical> <command> <arguments> `
  - Function : Set a `<command>` with its `<arguments>` in a certain date and time to be executed 
`<executions>` times every `<periodical>` seconds, or only `<executions>` times if `<periodical>` is 0.

- Command : `fp_set_cmd_unix`
  - Parameters : `<unix_time> <executions> <periodical> <command> <arguments> `
  - Function : Set a `<command>` with its `<arguments>` in a certain UNIX Time to be executed 
`<executions>` times every `<periodical>` seconds, or only `<executions>` times if `<periodical>` is 0.

- Command : `fp_set_cmd_dt`
  - Parameters : `<period> <executions> <periodical> <command> <arguments> `
  - Function : Set a `<command>` with its `<arguments>` in `<period>` seconds from now to be executed 
`<executions>` times every `<periodical>` seconds, or only `<executions>` times if `<periodical>` is 0.

##### Notes:
- If the command to set requires no arguments, `<arguments>` can be left empty.
- All characters following the space (` `) after the `command` field are considered command arguments  

##### Examples: 
For the next examples we assume that the time is 17:00 and the date is 26/12/2017 (December 26th, 2017) or 1514307600 in UNIX Time.

- Example 1: To set the command `com_ping` (that has the number `10` as argument) everyday (`86400` s) at 6am 
we need to write the line `fp_set_cmd 27 12 2017 06 00 00 1 86400 com_ping 10`

- Example 2: To set the command `help` (`help` has not arguments) at 8pm in UNIX Time (`1514318400`) we need to write
the line `fp_set_cmd_unix 1514318400 1 0 help`

- Example 3: To set the command `com_send_cmd <node> <command>` to node `1` with command `tm_send_status 1` (that is an integer and a string as arguments) in `30` seconds from now 
need to write the line `fp_set_cmd_dt 30 1 0 com_send_cmd 1 tm_send_status 1`

#### Deleting a command from the flight plan

- Command : `fp_del_cmd`
  - Parameters : `<day> <month> <year> <hour> <min> <sec>`
  - Function : Delete the command to be executed in the given time and date.
  
- Command : `fp_del_cmd_unix`
  - Parameters : `<unix_time>`
  - Function : Delete the command to be executed in the given UNIX Time.

##### Example
For the next examples we assume that the table has the commands set in the above section

- Example 1: To delete the `com_ping` command from the table we need to write the 
line `fp_del_cmd 27 12 2017 06 00 00`

- Example 2: To delete the `help` command from the table using the UNIX Time we need to write the 
line `fp_del_cmd_unix  1514318400`

#### Printing the flight plan table

- Command : `fp_show`
 - Parameters : This command has not parameters
 - Function : Print the flight plan table, if the flight plan table is empty it prints nothing 
(A `Flight plan table empty` message prints to the `INFO` log).

#### Resetting the flight plan

- Command : `fp_reset`
 - Parameters : This command has not parameters
 - Function : Drops the table and creates a new and empty table for the fight plan. This action cannot be undone.

