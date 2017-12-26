# Commands

## Commands interface

The public API and documentation for commands usage can be found in the 
`repoCommand.h` file. 

A command is simply a function that receives three parameters (2 char pointers 
and 1 integer) and returns an integer. The `cmdFunction` type is defined
as follows:

```c
typedef int (*cmdFunction)(char *fmt, char *params, int nparams);
```

For example, the following function is a command

```c
int foo(char* fmt, char* params, int nparams)
{
    printf("Hello world");
    return 1;
}
```

Commands can receive an arbitrary number of parameters in a string. For example, 
to call the command `bar` with parameters `3` and `hi` the call is:

```c
int rc = bar("%d %s", "3 hi", 2); 
```

In the implementation, developers must parse the parameters with `scanf` or a 
similar function:

```c
int bar(char* fmt, char* params, int nparams)
{
    int n;
    char s[3];
    
    if(sscanf(params, fmt, &n, &s) == nparams)
    {
        printf("Parsed: %d, %s", n, s);
        return 1;
    }
    else
    {
        printf("Failed parsing parameters"):
        return 0;
    }
}
``` 

The command repository (`repoCommand.c\h`) manages the commands available in
the system and provides an API to register, create and fill parameters, among
others. 

## Creating new commands

New commands are created in an existing or new file named as `cmdABC.c\h`. As
mentioned above, commands are just functions that share the same interface. In 
this example we will create the command **foo** that **receives a number and 
prints the double of this number:**

First, we create a function that receives the parameters as a string, the format
of the parameters as a string and the number of parameters as a integer, because
this command should be called as: `int rc = foo("%d", "2", 1)`:

```c
int foo(char* fmt, char* params, int nparams)
{
    int n;
    
    if(sscanf(params, fmt, &n) == nparams)
    {
        ind d = n*2
        printf("Double of %d id %d", n, d);
        return 1;
    }
    
    return 0;
}
```

Then, this command has to be registered in the command repository to be available
in the system. We use the `cmd_add` function, available in `repoCommand.h`,
inside an initialization function to register the command name, the function,
the parameters format and the number of parameters:

<<<<<<< HEAD
## Sending remote commands
=======
```c
int cmd_foo_init(void)
{
    cmd_add("foo", foo, "%d", 1);
}
```

Finally, the previous function have to be called inside the `cmd_repo_init`
function. The system automatically initializes the command repository by calling
that function.

```c
int cmd_repo_init(void)
{
    ...
    
    // Init repos
    test_cmd_init();
    ...
    ...
    ...
    cmd_foo_init(); // Call your initialization function here

    ...
    
    return CMD_OK;
}
```

## Executing commands

Once implemented and registered, commands can be searched and executed using
the command repository API. Usually in a client module (example 
`taskHousekeeping.c`) we include `repoCommand.h` to get, fill parameters and
request commands execution:

1. Get a new command by name
```c
cmd_t *new_cmd = cmd_get_str("foo");
```

2. Add parameters to the command
```c
cmd_add_params_var(new_cmd, 2);
```

3. Request command execution
```c
cmd_send(new_cmd, 2);
```
>>>>>>> 7c081529258f4526a2d7a4da634335d74f13d294

### Technical notes
1. `cmd_get_str` and `cmd_get_idx` use **malloc** inside to create the command.
The system itself executes the corresponding **free**.
2. Because of (1), it is not possible to execute more than one `cmd_send` call 
using the same command.

    - Wrong way
    ```c
    cmd_t *new_cmd = cmd_get_str("foo");
    for(int i=0; i<10; i++)
    {
        cmd_add_params_var(new_cmd, i);
        cmd_send(new_cmd, 2);
    }
    ```
    
    - Correct way
    ```c
    for(int i=0; i<10; i++)
    {
        cmd_t *new_cmd = cmd_get_str("foo");
        cmd_add_params_var(new_cmd, i);
        cmd_send(new_cmd, 2);
    }
    ```
