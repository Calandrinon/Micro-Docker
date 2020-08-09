#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


char** create_exec_arguments(int argc, char* argv[]) {
    char** exec_args = (char**)malloc(sizeof(char*) * (argc - 1)); 

    for (int arg_index = 0; arg_index < argc - 2; arg_index++) {
        exec_args[arg_index] = argv[arg_index + 2];
    }

    exec_args[argc - 2] = NULL;
    return exec_args;
}


void run(int argc, char* argv[]) {
    char** args = create_exec_arguments(argc, argv);

    int pid = fork();
    if (pid == 0) {
        execvp(argv[2], args);
    }
    wait(0);
	printf("Process terminated!\n");
    free(args);
}


int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Usage: micro-docker [program] [parameters]\n");
        return 0;
    }

    if (!strcmp(argv[1], "run")) {
        run(argc, argv);
    } else {
        printf("Incorrect action");
    }

    return 0;
}
