# splatsh
A linux shell written in C.
I began this project to sharpen my skills in C and learn a bit more about how the linux operating system works on a deeper level. 

## Usage
The repository contains a prebuilt binary (./shell), which can be executed and used.
If you would like to compile the project yourself, you can use: 
`gcc main.c -o shell -lreadline && ./shell`

## Features and builtins
* builtin cd: change directory
* builtin pwd: print working directory
* builtin type: print the type and location of a command
* builtin echo: prints the input
* builtin history: prints the command history (stored locally in .sh_history)
* Support for external commands
* Parsing for single and double quotations

### Planned features
* up/down arrow to navigate command history
* tab complete
* Multi-command pipelines 
* Output redirection (">" and ">>")
