#include <bits/posix1_lim.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

//HELPER FUNCTIONS

char *find_command(char *command, char *path) {
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

//BUILTIN FUNCTIONS
void builtin_exit(char **args) {
    exit(0);
}

void builtin_echo(char **args) {
    for (int j = 1; args[j] != NULL; j++) {
        printf("%s ", args[j]);
    }
    printf("\n");
}
void builtin_type(char **args) {
    char *exec_path = find_command(args[0], strdup(getenv("PATH")));
    if (exec_path != NULL) {
        printf("%s is %s\n", args[0], exec_path);
    }
    else {
        printf("%s: not found\n", args[1]);
    }
}

void builtin_pwd(char **args) {
    char cwd[PATH_MAX];
    printf("%s\n", getcwd(cwd, sizeof(cwd)));
}

void builtin_cd(char **args) {
    if(chdir(args[1]) != 0) {
        printf("cd: %s: No such file or directory\n", args[1]);
    }
}
//Defining struct, to store a name and a function
typedef struct {
    const char *name;
    void (*func)(char **args);
} Builtin;
//Table mappings to the functions
Builtin builtins[] = {
    {"exit", builtin_exit},
    {"echo", builtin_echo},
    {"type", builtin_type},
    {"pwd", builtin_pwd},
    {"cd", builtin_cd},
    {NULL, NULL}
};

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

    // Run cd
    //Run builtin command
    int found = 0;
    for (int i = 0; builtins[i].name; i++) {
        if (!strcmp(args[0], builtins[i].name)) {
            builtins[i].func(args);
            found = 1;
            break;
        }
    }
    if (!found){
        char *exec_path = find_command(args[0], strdup(getenv("PATH")));
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
