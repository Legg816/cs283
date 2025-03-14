#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    command_list_t clist;
    int rc;
    int lastRc;

    while (1) {
        printf("%s", SH_PROMPT);

        // Read the command line
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove the trailing newline
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strlen(cmd_buff) == 0) {
            continue;
        }

        // Check for the "exit" command
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            exit(0);
        }

        // Parse the command buffer into a command list
        rc = build_cmd_list(cmd_buff, &clist);
        if (rc == ERR_MEMORY) {
            return rc;
        }
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }
        if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }

        lastRc = lastRc;

        // Execute the parsed commands with pipes
        rc = execute_pipeline(&clist);
        if (rc) {
            if (rc == EPERM) {
                printf("Not permitted\n");
            } else if (rc == ENOENT) {
                printf("Command not found in PATH\n");
            } else if (rc == EACCES) {
                printf("Permission denied\n");
            } else {
                printf("Command failed with exit code: %d\n", rc);
            }
        }

        lastRc = rc;
    }

    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    int pipefds[2 * (num_cmds - 1)];

    // Create all pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe");
            return ERR_MEMORY;
        }
    }

    // Fork processes for each command
    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();

        if (pid == 0) {  // Child process
            // If it's not the first command, get input from previous pipe
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2 input");
                    exit(ERR_EXEC_CMD);
                }
            }

            // If it's not the last command, send output to next pipe
            if (i < num_cmds - 1) {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2 output");
                    exit(ERR_EXEC_CMD);
                }
            }

            // Handle output redirection for the last command
            if (clist->commands[i].output_file != NULL) {
                int fd;
                if (clist->commands[i].append_mode) {
                    fd = open(clist->commands[i].output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                } else {
                    fd = open(clist->commands[i].output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }

                if (fd == -1) {
                    perror("Error opening file for redirection");
                    exit(ERR_EXEC_CMD);
                }

                // Redirect output to the file
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Error redirecting output to file");
                    exit(ERR_EXEC_CMD);
                }
                close(fd);  // Close the file descriptor after duplicating
            }

            // Close all pipe file descriptors in the child process
            for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                close(pipefds[j]);
            }

            // Execute the command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
    }

    // Parent process: close all pipes and wait for children
    for (int i = 0; i < 2 * (num_cmds - 1); i++) {
        close(pipefds[i]);
    }

    for (int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }

    return OK;
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid == 0) {  // Child process
        if (cmd->output_file != NULL) {  // If there is an output file for redirection
            int fd;
            if (cmd->append_mode) {  // Append mode
                fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {  // Overwrite mode
                fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }

            if (fd == -1) {
                perror("Error opening file for redirection");
                exit(ERR_EXEC_CMD);
            }

            // Redirect output to the file
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Error redirecting output to file");
                exit(ERR_EXEC_CMD);
            }
            close(fd);  // Close the file descriptor after duplicating
        }

        execvp(cmd->argv[0], cmd->argv);  // Execute command
        _exit(errno);  // If execvp fails, pass the error to the parent
    } else if (pid > 0) {  // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                errno = exit_code;  // Set errno to the child's exit code
                return ERR_EXEC_CMD;
            }
        }
        return OK;
    } else {  // fork() failed
        perror("fork");
        return ERR_EXEC_CMD;
    }
}



// Function to parse the command buffer and detect redirection
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || !cmd_buff) return ERR_MEMORY;

    cmd_buff->_cmd_buffer = strdup(cmd_line);  // Duplicate command line
    if (!cmd_buff->_cmd_buffer) return ERR_MEMORY;

    cmd_buff->argc = 0;
    char *ptr = cmd_buff->_cmd_buffer;
    cmd_buff->output_file = NULL;
    cmd_buff->append_mode = false;

    while (*ptr) {
        while (isspace((unsigned char)*ptr)) ptr++;  // Skip spaces
        if (*ptr == '\0') break;

        // Handle redirection
        if (*ptr == '>') {
            ptr++;  // Skip '>'
            while (isspace((unsigned char)*ptr)) ptr++;  // Skip spaces after '>'
            if (*ptr == '>') {
                cmd_buff->append_mode = true;
                ptr++;  // Skip '>'
            }

            cmd_buff->output_file = ptr;  // Set the output file
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;  // Skip to next space
            if (*ptr) *ptr++ = '\0';  // Null-terminate the output file path
            continue;
        }

        // Normal argument processing
        cmd_buff->argv[cmd_buff->argc++] = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;  // Skip until next space
        if (*ptr) *ptr++ = '\0';  // Null-terminate the argument
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;  // Null-terminate argument list
    return (cmd_buff->argc > 0) ? OK : WARN_NO_CMDS;
}

// Updated free_cmd_list to properly free memory
int free_cmd_list(command_list_t *cmd_lst) {
    if (!cmd_lst) return ERR_MEMORY;
    for (int i = 0; i < cmd_lst->num; i++) {
        free(cmd_lst->commands[i]._cmd_buffer);  // Free each command buffer
    }
    cmd_lst->num = 0;  // Reset the number of commands
    return OK;
}

// Updated exec_built_in_cmd to handle errors
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (!cmd || cmd->argc == 0) return BI_NOT_BI;

    if (strcmp(cmd->argv[0], "exit") == 0) exit(0);  // Exit the shell
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) perror("cd");
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;  // Not a built-in command
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (!cmd_line || !clist) {
        return ERR_MEMORY;
    }

    clist->num = 0;
    char *cmd_copy = strdup(cmd_line);  // Duplicate the input command line
    if (!cmd_copy) {
        return ERR_MEMORY;
    }

    char *token = strtok(cmd_copy, PIPE_STRING);  // Split commands by pipe
    while (token != NULL) {
        if (clist->num >= CMD_MAX) {
            free(cmd_copy);
            return ERR_TOO_MANY_COMMANDS;  // Too many commands
        }

        if (build_cmd_buff(token, &clist->commands[clist->num]) != OK) {
            free(cmd_copy);
            return ERR_EXEC_CMD;  // Error parsing command
        }

        clist->num++;
        token = strtok(NULL, PIPE_STRING);  // Get next command
    }

    free(cmd_copy);
    return (clist->num > 0) ? OK : WARN_NO_CMDS;
}
