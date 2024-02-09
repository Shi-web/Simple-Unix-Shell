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
            write(STDERR_FILENO, error_message, strlen(error_message)); //perror("Error opening file");
            exit(EXIT_FAILURE);
        }
    }

    while (1)
    {
        if (file == NULL)
        {
            printf("msh> ");
            while (!fgets(command_string, MAX_COMMAND_SIZE, stdin));
        }
        else
        {
            if (fgets(command_string, MAX_COMMAND_SIZE, file) == NULL)
            {
                fclose(file);
                break;
            }
        }

        // Process the command
        process_command(command_string);
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

    while (((argument_pointer = strsep(&working_string, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
        token[token_count] = strndup(argument_pointer, MAX_COMMAND_SIZE);
        if (strlen(token[token_count]) == 0)
        {
            token[token_count] = NULL;
        }
        token_count++;
    }

    int found = 0;
    int builtIN = 0;

    if (strcmp(token[0], "exit") == 0)
    {
        exit(0);
    }
    if (strcmp(token[0], "cd") == 0)
    {
        builtIN = 1;
        chdir(token[1]);
    }

    char filename[MAX_COMMAND_SIZE];
    strcpy(filename, token[0]);
    char path[MAX_COMMAND_SIZE];
    for (int i = 0; i < sizeof(directories) / sizeof(directories[0]); ++i)
    {
        char full_path[256];
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
        write(STDERR_FILENO, error_message, strlen(error_message)); //printf("Error: File '%s' not found in any of the specified directories.\n", filename);
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        write(STDERR_FILENO, error_message, strlen(error_message)); //perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        for (int i = 0; i < MAX_NUM_ARGUMENTS; i++)
        {
            if (token[i] == NULL)
            {
                break;
            }

            if (strcmp(token[i], ">") == 0)
            {
                int fd = open(token[i + 1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                if (fd < 0)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message)); //perror("Can't open output file.");
                    exit(0);
                }
                dup2(fd, 1);
                close(fd);

                token[i] = NULL;
            }
        }
        if ((!builtIN) && (found))
        {
            if (execv(path, token) == -1)
            {
                write(STDERR_FILENO, error_message, strlen(error_message)); //perror("execv");
                exit(0);
            }
        }
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }

    free(head_ptr);

    for (int i = 0; i < token_count; i++)
    {
        free(token[i]);
        token[i] = NULL;
    }
}
