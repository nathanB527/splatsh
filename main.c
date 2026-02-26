#include <bits/posix1_lim.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
char *find_command(char *command, char *path) {
    printf("path is %s\n", path);
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

    //Run builtin command
    char builtin_path[256] = "/home/nathan/Projects/splatsh/builtins/bin";
    char *exec_path = find_command(args[0], strdup(builtin_path));
    if (exec_path != NULL) {
    }
    else {
    exec_path = find_command(args[0], strdup(getenv("PATH")));
    }


    //Run external command
    //Use fork to create a child process before calling execv
    if (exec_path != NULL) {
        pid_t pid = fork();
        if (pid == 0 ) {
            execv(exec_path, args);
        }
        else {
            wait(NULL);
        }}
    else {
        printf("%s: not found\n", input);
    }
  }
  return 0;
}
