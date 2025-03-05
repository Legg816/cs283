#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

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
int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    command_list_t cmd_list;
    int rc;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strlen(cmd_buff) == 0) {
            continue;
        }

        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }

        rc = build_cmd_list(cmd_buff, &cmd_list);
        if (rc != OK) {
            switch (rc) {
                case WARN_NO_CMDS:
                    printf(CMD_WARN_NO_CMD);
                    break;
                case ERR_TOO_MANY_COMMANDS:
                    printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                    break;
                case ERR_MEMORY:
                    fprintf(stderr, "Memory allocation error\n");
                    break;
                default:
                    fprintf(stderr, "Unknown error: %d\n", rc);
                    break;
            }
            continue;
        }

        rc = execute_pipeline(&cmd_list);
        if (rc != OK) {
            fprintf(stderr, "Pipeline execution error: %d\n", rc);
        }

        free_cmd_list(&cmd_list);
    }

    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || !cmd_buff) {
        return ERR_MEMORY;
    }

    // Allocate and copy the command buffer
    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }

    cmd_buff->argc = 0;
    char *ptr = cmd_buff->_cmd_buffer;
    bool in_quotes = false;

    while (*ptr) {
        // Skip leading spaces unless inside quotes
        while (isspace((unsigned char)*ptr) && !in_quotes) ptr++;

        if (*ptr == '\0') break;

        // Handle quoted arguments
        if (*ptr == '"') {
            in_quotes = !in_quotes;
            ptr++;
            cmd_buff->argv[cmd_buff->argc++] = ptr;
            while (*ptr && (in_quotes || !isspace((unsigned char)*ptr))) {
                if (*ptr == '"') {
                    *ptr = '\0';
                    in_quotes = false;
                    break;
                }
                ptr++;
            }
        } else {
            // Normal argument parsing
            cmd_buff->argv[cmd_buff->argc++] = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        }

        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
    }

    cmd_buff->argv[cmd_buff->argc] = NULL; // Null-terminate the argument list

    return (cmd_buff->argc > 0) ? OK : WARN_NO_CMDS;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (!cmd_line || !clist) {
        return ERR_MEMORY;
    }

    clist->num = 0;
    char *cmd_copy = strdup(cmd_line);
    if (!cmd_copy) {
        return ERR_MEMORY;
    }

    char *token = strtok(cmd_copy, PIPE_STRING);
    while (token != NULL) {
        if (clist->num >= CMD_MAX) {
            free(cmd_copy);
            return ERR_TOO_MANY_COMMANDS;
        }

        if (build_cmd_buff(token, &clist->commands[clist->num]) != OK) {
            free(cmd_copy);
            return ERR_CMD_ARGS_BAD;
        }

        clist->num++;
        token = strtok(NULL, PIPE_STRING);
    }

    free(cmd_copy);
    return (clist->num > 0) ? OK : WARN_NO_CMDS;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    }
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    }
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (!cmd || cmd->argc == 0) {
        //return BI_RC;
    }

    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);

    switch (cmd_type) {
        case BI_CMD_EXIT:
            exit(0);
        case BI_CMD_CD:
            if (cmd->argc > 1) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd");
                }
            }
            return BI_EXECUTED;
        case BI_CMD_DRAGON:
            //print_dragon();  // Assuming this function exists
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(ERR_EXEC_CMD);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    } else {
        perror("fork");
        return ERR_EXEC_CMD;
    }
}

int execute_pipeline(command_list_t *clist) {
    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }

    int pipes[CMD_MAX - 1][2];  // Pipes for inter-process communication
    pid_t pids[CMD_MAX];         // Store PIDs for waitpid

    // Create pipes
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            return ERR_MEMORY;
        }
    }

    for (int i = 0; i < clist->num; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return ERR_MEMORY;
        }

        if (pid == 0) {  // Child process
            // Redirect input if not the first command
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // Redirect output if not the last command
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipe ends in child
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute the command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");  // If execvp fails
            exit(ERR_EXEC_CMD);
        } else {  // Parent process
            pids[i] = pid;
        }
    }

    // Close all pipes in parent
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < clist->num; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    return OK;
}


int free_cmd_list(command_list_t *cmd_lst) {
    if (!cmd_lst) return ERR_MEMORY;
    for (int i = 0; i < cmd_lst->num; i++) {
        //free_cmd_buff(&cmd_lst->commands[i]);
    }
    return OK;
}

