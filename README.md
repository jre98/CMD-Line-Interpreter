# Assignment 2 - Command Line Interpreter

This is a command line interpreter that I wrote for my Computer Operating Systems Class. The program
parses user input and then executes the command in a child thread. One issue that I was not able to
resolve in time for submission is that, if an exception is thrown, the program automatically aborts 
due to the uncaught exception error. I tried to find a workaround for it, but everything else I tried,
such as catching the exception just broke the program more, so I figured it wasn't worth it.
