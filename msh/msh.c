// The MIT License (MIT)
// 
// Copyright (c) 2024 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 32  

int check_executable_file(const char *filename) // this function checks if a particular file exists in a directory and is executable
{
    if (access(filename, X_OK) == 0) {
        //printf("File '%s' is executable.\n", filename);
        return 1;  // File is executable
    } else {
        //printf("File '%s' is not executable or does not exist.\n", filename);
        return 0;  // File is not executable or does not exist
    }
}

int main( int argc, char * argv[] )
{
  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  const char *directories[] = {"/bin", "/usr/bin", "/usr/local/bin", "./"};

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandi line.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something.
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_pointer;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    
    char *head_ptr = working_string;
    
    // Tokenize the input with whitespace used as the delimiter
    while ( ( (argument_pointer = strsep(&working_string, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_pointer, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    int found = 0;  // Flag to check if the file is found
    int builtIN = 0;// Flag to check if the its a built-in command
    // \TODO Remove this code and replace with your shell functionality
     
    if (strcmp(token[0], "exit")==0)
    {
       exit(0);
    }
    if (strcmp(token[0],"cd")==0)
    {
      builtIN=1;
      chdir(token[1]);

      
    }
    
    char filename[MAX_COMMAND_SIZE]; //for command that user inputs
    strcpy(filename, token[0]);
    char path[MAX_COMMAND_SIZE];
    for (int i = 0; i < sizeof(directories) / sizeof(directories[0]); ++i) {
        // Construct the full path by concatenating the directory and filename
        char full_path[256];  // Adjust the size as needed
        snprintf(full_path, sizeof(full_path), "%s/%s", directories[i], filename);

        // Check if the file exists and is executable in the current directory
        if (check_executable_file(full_path)) {
            found = 1;
            strcpy(path,full_path);
            break;  // Stop searching if the file is found
        }
    }

    if ((!found)&&(!builtIN))
    {
        printf("Error: File '%s' not found in any of the specified directories.\n", filename);
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
          for (int i = 0; i < MAX_NUM_ARGUMENTS; i++) {
                    if (token[i] == NULL) {
                        break;
                    }

                    if (strcmp(token[i], ">") == 0) {
                        int fd = open(token[i + 1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        if (fd < 0) {
                            perror("Can't open output file.");
                            exit(0);
                        }
                        dup2(fd, 1);
                        close(fd);

                        // Trim off the > output part of the command
                        token[i] = NULL;
                    }
                }
        if ((!builtIN)&&(found)){
          execv(path, token);
          perror("execv");
          exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }


    free( head_ptr );
    // Empty the array
    for (int i = 0; i < token_count; i++) {
        free(token[i]);  // Free memory occupied by each string
        token[i] = NULL;  // Set the pointer to NULL
    }


  }
  
}

