#include <bits/posix1_lim.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
char *find_command(char *command) {
    //Create path pointer, for loop that iterates over all DIRectories.
    char *path = strdup(getenv("PATH"));
    for (char *dir = strtok(path, ":"); dir != NULL;
        dir = strtok(NULL, ":")) {
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, command);
            if (access(fullpath, X_OK) == 0){
                free(path);
                return strdup(fullpath);
            }
        }
    free(path);
    return NULL;
}


int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  //Define user and host names
  char hostname[HOST_NAME_MAX];
  char *username;
  username = getenv("USER");
  if (gethostname(hostname, HOST_NAME_MAX) == 0) {
  }

  while (1) {
    char cwd[PATH_MAX];
    printf("[%s@%s %s]$ ", username, hostname, getcwd(cwd, sizeof(cwd)));
    // Recieve input from stdin, write to input

    char input[1024];
    fgets(input, sizeof(input), stdin);
    //Remove trailing new line character from input
    input[strlen(input)-1] = '\0';
    //create helper pointers. builtin gives the first word of the input, arg gives the rest of the input.
    char *command = strtok(input, " ");
    char *args_str = strtok(NULL, "");
    //parse arguments into an array
    char *args[64];
    args[0] = command;
    int i = 0;
    char *token = strtok(args_str, " ");
    while (token != NULL) {
        args[i+1] = token; i++;
        token = strtok(NULL, " ");
    }
    args[i+1] = NULL;

    //Built in functions: exit, echo, type, pwd, cd
    if (strcmp(command, "exit") == 0)
      break;
    else if (strcmp(command, "echo") == 0){
    //ECHO, returns args
        for (int j = 1; args[j] != NULL; j++) {
            printf("%s ", args[j]);
        }
        printf("\n");
    } else if (strcmp(command, "type") == 0) {
    //TYPE, returns where a command is from
        if (!strcmp(args[1], "exit") || !strcmp(args[1], "echo") || !strcmp(args[1], "type") || !strcmp(args[1], "pwd") || !strcmp(args[1], "cd")) {
        printf("%s is a shell builtin\n", args_str);
        }
        else {
            char *exec_path = find_command(args_str);
            if (exec_path != NULL) {
                printf("%s is %s\n", args_str, exec_path);
            }
            else {
                printf("%s: not found\n", args[1]);
            }

        }
    } else if (strcmp(command, "pwd") == 0) {
            char cwd[PATH_MAX];
            printf("%s\n", getcwd(cwd, sizeof(cwd)));

    } else if (strcmp(command, "cd") == 0) {
        if(chdir(args[1]) != 0) {
            printf("cd: %s: No such file or directory\n", args[1]);
        }
    }

    //Run an external command
    else {
        char *exec_path = find_command(command);
        //Use fork to create a child process before calling execv
        if (exec_path != NULL) {
            pid_t pid = fork();
            if (pid == 0 ) {
                execv(exec_path, args);
            }
            else {
                wait(NULL);
            }
        }
        else {
            printf("%s: not found\n", input);
        }
    }
  }
return 0;
}
