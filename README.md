[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/QgP-rrgn)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-7f7980b617ed060a017424585567c406b6ee15c891e84e1186181d67ecf80aa0.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=13650984)

# Unix Shell

In this project, you'll build a simple Unix shell. The shell is the heart of
the command-line interface, and thus is central to the Unix/C programming
environment. Mastering use of the shell is necessary to become proficient in
this world; knowing how the shell itself is built is the focus of this
project.

There are three specific objectives to this assignment:

* To further familiarize yourself with the Linux programming environment.
* To learn how processes are created, destroyed, and managed.
* To gain exposure to the necessary functionality in shells.

## Overview

In this assignment, you will implement a *command line interpreter (CLI)* or,
as it is more commonly known, a *shell*. The shell should operate in this
basic way: when you type in a command (in response to its prompt), the shell
creates a child process that executes the command you entered and then prompts
for more user input when it has finished.

The shells you implement will be similar to, but simpler than, the one you run
every day in Unix. If you don't know what shell you are running, it's probably
`bash`. One thing you should do on your own time is learn more about your
shell, by reading the man pages or other online materials.

### Source Code Directories
msh - contains the file msh.c which you will modify for this assignment. No
      other file may be modified

tester - test harness.  No file in this directory may be modified

### Building the Shell
You can build the code by typing:
```
make
```
### Testing the Shell
You can run the provided tests by typing:
```
./test-msh.sh
```
## Program Specifications

### Submission

Your code will be submitted by pushing your code to main.  Any other branches
you have will not be graded.

### Development Environment

You code will be tested on a GitHub codespace.  Code that does not compile
or run on the provided codespace will result in a grade of 0.

### Basic Shell: `msh`

Your basic shell, called `msh` is basically an interactive loop: it repeatedly 
prints a prompt `msh> ` (note the space after the greater-than sign), parses the 
input, executes the command specified on that line of input, and waits for 
the command to finish. This is repeated until the user types `exit`.  The 
name of your final executable should be `msh`.

The shell can be invoked with either no arguments or a single argument;
anything else is an error. Here is the no-argument way:

```
prompt> ./msh
msh> 
```

At this point, `msh` is running, and ready to accept commands. Type away!

The mode above is called *interactive* mode, and allows the user to type
commands directly. The shell also supports a *batch mode*, which instead reads
input from a file and executes commands from therein. Here is how you
run the shell with a batch file named `batch.txt`:

```
prompt> ./msh batch.txt
```

One difference between batch and interactive modes: in interactive mode, a
prompt is printed (`msh> `). In batch mode, no prompt should be printed.

You should structure your shell such that it creates a process for each new
command (the exception are *built-in commands*, discussed below).  Your basic
shell should be able to parse a command and run the program corresponding to
the command.  For example, if the user types `ls -la /tmp`, your shell should
run the program `/bin/ls` with the given arguments `-la` and `/tmp` 

## Structure

### Basic Shell

The shell is very simple (conceptually): it runs in a while loop, repeatedly
asking for input to tell it what command to execute. It then executes that
command. The loop continues indefinitely, until the user types the built-in
command `exit`, at which point it exits. That's it!

For reading lines of input, you should use `getline()`. This allows you to
obtain arbitrarily long input lines with ease. Generally, the shell will be
run in *interactive mode*, where the user types a command (one at a time) and
the shell acts on it. However, your shell will also support *batch mode*, in
which the shell is given an input file of commands; in this case, the shell
should not read user input (from `stdin`) but rather from this file to get the
commands to execute.

In either mode, if you hit the end-of-file marker (EOF), you should call
`exit(0)` and exit gracefully. 

To parse the input line into constituent pieces, you might want to use
`strsep()`. Read the man page (carefully) for more details.

To execute commands, look into `fork()`, `exec()`, and `wait()/waitpid()`.
See the man pages for these functions, and also read the relevant [book
chapter](http://www.ostep.org/cpu-api.pdf) for a brief overview.

You will note that there are a variety of commands in the `exec` family; for
this project, you must use `execv`. You should **not** use the `system()`
library function call to run a command.  Any use of system will result in
a grade of 0.  Remember that if `execv()` is successful, it will not return; 
if it does return, there was an error (e.g.,the command does not exist). 
The most challenging part is getting the arguments correctly specified. 

### Paths

**Important:** Note that the shell itself does not *implement* `ls` or other
commands (except built-ins). All it does is find those executables in one of
the directories specified by `path` and create a new process to run them.

To check if a particular file exists in a directory and is executable,
consider the `access()` system call. For example, when the user types `ls`,
and path is set to include both `/bin` and `/usr/bin`, try `access("/bin/ls",
X_OK)`. If that fails, try "/usr/bin/ls". If that fails too, it is an error.

Your shell path should search four directories: `/bin` '/usr/bin' '/usr/local/bin'
and './'

### Built-in Commands

Whenever your shell accepts a command, it should check whether the command is
a **built-in command** or not. If it is, it should not be executed like other
programs. Instead, your shell will invoke your implementation of the built-in
command. For example, to implement the `exit` built-in command, you simply
call `exit(0);` in your msh source code, which then will exit the shell.

In this project, you should implement `exit`, `cd`, and `path` as built-in
commands.

* `exit`: When the user types `exit`, your shell should simply call the `exit`
  system call with 0 as a parameter. It is an error to pass any arguments to
  `exit`. 

* `cd`: `cd` always take one argument (0 or >1 args should be signaled as an
error). To change directories, use the `chdir()` system call with the argument
supplied by the user; if `chdir` fails, that is also an error.

### Redirection

Many times, a shell user prefers to send the output of a program to a file
rather than to the screen. Usually, a shell provides this nice feature with
the `>` character. Formally this is named as redirection of standard
output. To make your shell users happy, your shell should also include this
feature, but with a slight twist (explained below).

For example, if a user types `ls -la /tmp > output`, nothing should be printed
on the screen. Instead, the standard output of the `ls` program should be
rerouted to the file `output`. In addition, the standard error output of
the program should be rerouted to the file `output` (the twist is that this
is a little different than standard redirection).

If the `output` file exists before you run your program, you should simple
overwrite it (after truncating it).  

The exact format of redirection is a command (and possibly some arguments)
followed by the redirection symbol followed by a filename. Multiple
redirection operators or multiple files to the right of the redirection sign
are errors.

Note: don't worry about redirection for built-in commands (e.g., we will
not test what happens when you type `path /bin > file`).

Hint: [popen.c](https://github.com/CSE3320-Spring-2024/Code-Samples/blob/main/popen.c)
in the Code-Samples repo demonstrates how to do a redirection


### Program Errors

**The one and only error message.** You should print this one and only error
message whenever you encounter an error of any type:

```
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
```

The error message should be printed to stderr (standard error), as shown
above. 

After most errors, your shell simply *continue processing* after
printing the one and only error message. However, if the shell is
invoked with more than one file, or if the shell is passed a bad batch
file, it should exit by calling `exit(1)`.

There is a difference between errors that your shell catches and those that
the program catches. Your shell should catch all the syntax errors specified
in this project page. If the syntax of the command looks perfect, you simply
run the specified program. If there are any program-related errors (e.g.,
invalid arguments to `ls` when you run it, for example), the shell does not
have to worry about that (rather, the program will print its own error
messages and exit).


### Miscellaneous Hints

Remember to get the **basic functionality** of your shell working before
worrying about all of the error conditions and end cases. For example, first
get a single command running (probably first a command with no arguments, such
as `ls`). 

Next, add built-in commands. Then, try working on redirection. 

At some point, you should make sure your code is robust to white space of
various kinds, including spaces (` `) and tabs (`\t`). In general, the user
should be able to put variable amounts of white space before and after
commands, arguments, and various operators; however, the 
redirection operator does not require whitespace.

Check the return codes of all system calls from the very beginning of your
work. This will often catch errors in how you are invoking these new system
calls. It's also just good programming sense.

Beat up your own code! You are the best (and in this case, the only) tester of
this code. Throw lots of different inputs at it and make sure the shell
behaves well. Good code comes through testing; you must run many different
tests to make sure things work as desired. Don't be gentle -- other users
certainly won't be. 

