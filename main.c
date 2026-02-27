#include <bits/posix1_lim.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
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

int parse_args(char *input, char **args) {
    int i = 0;
    char *p = input;
    while (*p) {
        while (*p == ' ') p++;
        if (*p == '\0') break;

        if (*p == '"' || *p == '\'') { // Checks for double quotes or single quotes
            char quote = *p++;
            args[i++] = p;
            while (*p && *p != quote) p++; //scan til closing quote
            if (*p){ *p++ = '\0';}


        } else { //Normal argument parsing
            args[i++] = p;
            while (*p && *p != ' ') {p++;} //scan til space
            if (*p) *p++ = '\0';
        }
    }
    args[i] = NULL;
    return i;
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

void builtin_history(char **args) {
    HIST_ENTRY **hist = history_list();
    if (hist) {
        for (int i = 0; hist[i]; i++) {
            printf("%d: %s\n", i+1, (*hist[i]).line);
        }
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
    {"history", builtin_history},
    {NULL, NULL}
};
int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  //Initialize history
  char *hist_file = ".sh_history";
 if (read_history(hist_file) == 0) {}


  //Define user and host names
  char hostname[HOST_NAME_MAX];
  char *username;
  username = getenv("USER");
  if (gethostname(hostname, HOST_NAME_MAX) == 0) {}
  while (1) {
      char cwd[PATH_MAX];
      printf("[%s@%s %s]$ ", username, hostname, getcwd(cwd, sizeof(cwd)));
      // Recieve input from stdin, write to input
      char input[1024];
      fgets(input, sizeof(input), stdin);

      //Remove trailing new line character from input
      input[strlen(input)-1] = '\0';

      //Add and write to the history file.
      add_history(input);
      write_history(hist_file);


      //create helper pointers. builtin gives the first word of the input, arg gives the rest of the input.
      char *command = NULL;
      //parse arguments into an array
      char *args[64];
      parse_args(input, args);
      command = args[0];
      if (command == NULL) continue;

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
  write_history(hist_file);
  return 0;
}
