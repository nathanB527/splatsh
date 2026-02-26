// C program named mainreturn.c to demonstrate the working
// of command line arguement
#include <stdio.h>
#include <unistd.h>


// defining main with arguments
int main(int argc, char* argv[])
{
    if(chdir(argv[1]) != 0) {
        printf("cd: %s: No such file or directory\n", argv[1]);
    }
    printf("directory changed to %s", argv[1]);
}
