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

#define WHITESPACE " \t\n"
#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 32

void process_command(char *command_string);

int check_executable_file(const char *filename)
{
    if (access(filename, X_OK) == 0)
    {
        return 1; // File is executable
    }
    else
    {
        return 0; // File is not executable or does not exist
    }
}

int main(int argc, char *argv[])
{
    char *command_string = (char *)malloc(MAX_COMMAND_SIZE);
    char error_message[30] = "An error has occurred\n";
    FILE *file = NULL; // File pointer for batch mode

    if (argc == 2)
    {
        file = fopen(argv[1], "r");
        if (file == NULL)
        {
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            exit(EXIT_FAILURE);
        }
    }

    while (1)
    {
        if (file == NULL)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(EXIT_FAILURE);
        }
        else if (file != NULL)
        {
            while (fgets(command_string, MAX_COMMAND_SIZE, file) != NULL)
            {
            
                    process_command(command_string);
                
            }
            if (feof(file))
            {
                exit(0);
            }
            
        }
        else
        {
            printf("msh> ");
            while (!fgets(command_string, MAX_COMMAND_SIZE, stdin));
                
            process_command(command_string);
            
        }

        
    }

    free(command_string);
    return 0;
}

void process_command(char *command_string)
{
    const char *directories[] = {"/bin", "/usr/bin", "/usr/local/bin", "./"};
    char error_message[30] = "An error has occurred\n";

    char *token[MAX_NUM_ARGUMENTS];
    int token_count = 0;
    char *argument_pointer;
    char *working_string = strdup(command_string);
    char *head_ptr = working_string;

    // Tokenize the input with whitespace used as the delimiter
        while (((argument_pointer = strsep(&working_string, WHITESPACE)) != NULL) &&
               (token_count < MAX_NUM_ARGUMENTS)) {
            if (strlen(argument_pointer) > 0) {
                token[token_count] = strdup(argument_pointer);
                token_count++;
            }
        }

        // Ensure the last element of the token is NULL
        token[token_count] = NULL;

    int found = 0;
    int builtIN = 0;
    if (token[0]==NULL) return;
    if (strcmp(token[0], "exit") == 0)
    {
        if( token[1] != NULL )
        {
            write(STDERR_FILENO, error_message, strlen(error_message)); 
        }
        exit(0);
    }
    else if (strcmp(token[0], "cd") == 0)
    {
        builtIN = 1;
        int x = chdir(token[1]);
        if (x != 0)
        {
            write(STDERR_FILENO, error_message, strlen(error_message)); // chdir failed
        }
    }

    else
    {
        char filename[MAX_COMMAND_SIZE];
        strcpy(filename, token[0]);
        char path[MAX_COMMAND_SIZE];
        for (int i = 0; i < sizeof(directories) / sizeof(directories[0]); ++i)
        {
            char full_path[257];
            snprintf(full_path, sizeof(full_path), "%s/%s", directories[i], filename);

            if (check_executable_file(full_path))
            {
                found = 1;
                strcpy(path, full_path);             
                break;
            }
        }

        if ((!found) && (!builtIN))
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        pid_t pid = fork();
        if (pid == -1)
        {
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        { 
            if ((!builtIN) && (found))
            {
                for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
                {
                    if (token[i] == NULL)
                    {
                        break;
                    }

                    if (strcmp(token[i], ">") == 0)
                    {
                        // Check if a file is provided after the redirection symbol
                         if (token[i + 1] == NULL || token[i + 2] != NULL) {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(0);
                        }

                        int fd = open(token[i + 1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        if (fd < 0)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message)); 
                            exit(0);
                        }
                        dup2(fd, 1);
                        close(fd);

                        token[i] = NULL;
                      
                      

                    }
                }
                if (execv(path, token) == -1)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message)); 
                    exit(0);
                }
            }
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
        }
    }

    free(head_ptr);

    for (int i = 0; i < token_count; i++)
    {
        free(token[i]);
        token[i] = NULL;
    }
}