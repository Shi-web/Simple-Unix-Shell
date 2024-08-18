# Unix Shell

This project involved building a simple Unix shell, which is the heart of the command-line interface and central to the Unix/C programming environment. Mastering the use of the shell is necessary for proficiency in this environment, and understanding how the shell itself is built was the focus of this project.

## Objectives

The primary goals of this project were to:

* Deepen familiarity with the Linux programming environment.
* Learn how processes are created, destroyed, and managed.
* Gain exposure to the necessary functionality in shells.

## Overview

The project involved implementing a *command line interpreter (CLI)*, commonly known as a *shell*. The shell operates by accepting a command, creating a child process to execute the command, and then prompting the user for more input once the command has completed.

This shell is similar to the one you run every day in Unix (likely `bash`), though it is simpler in design. Understanding how this shell is built provides insight into how more complex shells function.

### Source Code Directories

* `msh` - Contains the main implementation file `msh.c`.
* `tester` - Includes a test harness for the shell.

### Building the Shell

To build the shell, simply run:

```bash
make
```

### Running the Shell

To run the shell in interactive mode:

```bash
./msh
```

To run the shell in batch mode with a batch file:

```bash
./msh batch.txt
```

### Program Specifications

The shell, named `msh`, operates in an interactive loop, prompting the user with `msh> `, parsing the input, executing the command, and waiting for the command to finish. This continues until the user types `exit`.

The shell can be invoked with no arguments or with a single argument specifying a batch file. Any other usage results in an error.

### Features

1. **Interactive and Batch Modes**: The shell can operate interactively or read commands from a batch file.
  
2. **Command Execution**: The shell executes commands by creating child processes, using `fork()` and `execv()`.

3. **Built-in Commands**: 
   - `exit`: Exits the shell.
   - `cd`: Changes the current working directory.

4. **Redirection**: The shell supports redirection of output to files using the `>` character, with both standard output and standard error redirected to the specified file.

5. **Error Handling**: The shell provides consistent error messages and continues processing commands unless a critical error occurs.

### Testing

The provided tests can be run using:

```bash
./test-msh.sh
```

### Conclusion

This project was an exercise in understanding and implementing core Unix shell functionality, providing a foundation for further exploration of Unix programming and shell environments. Through the development of this shell, I gained a deeper understanding of process management, command execution, and the intricacies of the Linux programming environment. 
