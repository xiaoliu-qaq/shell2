# CS587 - Advanced Operating Systems Project 1: The `ish` shell

## Due Date
  * Due Date: Monday, March 3, 2025, 11:59pm

## Overview
Your task is to implement---on your own---a basic Unix command shell. Your shell *must* use either the provided C (src-c/) or C++ parser (src/), and implement a subset of the features described in the included ISH manual page. Notably, you do *not* need to implement job control or pipelines. 

The basic features you need to implement are:
  * Basic command execution
    * Correct shell prompt as described in the manual page
    * Run command with full command name
    * Run command with full command name and arguments
    * 'cd' builtin works (check with /bin/pwd)
    * `quit` builtin and EOF cause shell to exit properly and cleanly

  * Envirionment and alias handling
    * Environment variables correctly passed to child
    * PATH searching works
    * PATH in wrong syntax handled well
    * `printenv` (`setenv` with no arguments) works
    * `unsetenv` works
    * Adding an alias works
    * Removing an alias works

  * Miscellaneous
    * .ishrc executed properly

  * File redirection
    * Redirect output to a simople file
    * Appending to a file works
    * Redirection of stdout and stderr works
    * Redirect input from a simple file
    * Ambiguous redirections are detected and reported
    * Redirection of both input and output works.

  * Error handling
    * The shell should fail gracefully and report errors like `csh` does when permissions or something else goes wrong doing thie things described above.
   
The features you do //not// need to implement that are described in the ISH man page are:
  * Job control (the & separator, ^Z handling, and the bg/fg/jobs builtins)
  * Pipelines

## Starter Source Code and Programming Assignment Restrictions 

Your shell should be written in C or C++, compile using cmake, and produce an executable named `ish`. I have provided a C++ parser for the subset of 'ish' assigned in this assignment in the src/ directory; the C++ starter code does *not* parse pipelines as they are not required for this assignment. In addition, I have also provided C starter code in the src-c director that you may use if you prefer. To use this code, change the CMakeLists.txt in the top-level directory to point to the src-c/ directory instead of the src/ to find its source code.

Your code must also be "clean" -- it should compile without warnings or unresolved references ion a standard UNIX system with the Boost C++ libraries installed. I will provide a GitHub Codespace with the appropriate setup for developing your program shortly after the assignment becomes available. I encourage you to use GitHub Codespaces for development, as if you have problems I can connect to your codespace and help you debug your program. Your programs are expected to be well organized and easy to read, as well as correct.

You //must//, use either the C++ or C parsers provided to implement your shell. You may modify these parsers if necessary (e.g. if you want to change the parser to detect builtin commands or add extra functionality to the command structure/class provided), but the general structure of the parser *must remain the same*. You may *not* use your own custom parser or one taken from elsewhere.

You must use the `fork` and `execve` C system calls to start new processes and the `dup` or `dup2` command to handle file redirection. You may *not* use other variants of `exec` or the `system` call, their C++ equivalents, or any other mechanism for creating processes or redirecting file I/O.

## Testing

Testcases that test correctness and constitute 85% of the grade of the shell will be provided approximately one week after the assignment is posted. Additional tests that test your shell error handling will be used for final testing ofyour program. A pull request will be submitted to your github repository adding test features when these testcases are available.

## Assignment submission

You will use GitHub classroom to submit a working program on or before the due date. Be sure to commit and push all of your changes to the `main` branch on your repository prior to the due date!

## Supporting and Reference Materials

Before starting, you should become familiar with the Unix system calls defined in Section 2. of the UNIX manual There are also several library routines in Section 3 that provide convenient interfaces to some of the more cryptic system calls. However, you may _not_ use the library routine `system` nor any of the routines prohibitied on the `ish` man page.  Several chapters of the Richard Stevens book _Advanced Programming in the UNIX Environment_ contain helpful information; Chapters 7, 8, and 9 are especially relevant, and this book is available online for free through the UNM library.

## Additional Advice

To implement `ish`, you will be creating a process that forks off other processes, which in turn forks off more processes, etc. If you inadvertently fork too many processes, you will cause Unix to run out, making yourself and everyone else on the machine very unhappy. _Be careful about this.

If you are in doubt about the functionality of `ish` or how it should behave in a particular situation, model the behavior on that of `csh`.  If you have specific questions about the project, ask in Discord. 

A few final bits of advice. First, and most importantly, get started early; you almost certainly have a lot to learn before you can start implementing anything.  Second, once you have a good understanding of what you are being asked to do, I strongly suggest that you develop a detailed design, implementation, and testing plan.  My personal style is to get functionality working one step at a time, for example, processing of simple commands, then environment handling and PATH searching, then I/O redirection. 
