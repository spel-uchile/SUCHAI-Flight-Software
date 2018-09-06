#Multiple Executers

##Why?

The reason to implement multiple executers is to execute more than a command
at the same time, running the two commands in different threads.

##Idea Behind

To implement this functionality was needed to change the communication
of two methods (taskDispatcher and taskExecuter), changing from 
a type of interruption to a pulling communication.

##Test

Here you can see two graphics, in each one the command stack start with
five commands to be executed.

The first one is with just one taskEcecuter and the second is with 2 
taskExecuters.

