#define _GNU_SOURCE 
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#define STACKSIZE (1024*1024)


typedef struct {
    char* name;
    char** arguments;
} Command;


char** create_exec_arguments(int argc, char* argv[]) {
	char** exec_args = (char**)malloc(sizeof(char*) * (argc - 1)); 

	for (int arg_index = 0; arg_index < argc - 2; arg_index++) {
		exec_args[arg_index] = argv[arg_index + 2];
    }

	exec_args[argc - 2] = NULL;
	return exec_args;
}


int execute_command(void* arg) {
	Command* command = (Command*)arg;
	execvp(command->name, command->arguments);
    return 0;
}


int child(void* arg) {
	char container_hostname[] = "insert_random_hostname";
	sethostname(container_hostname, strlen(container_hostname));

	void* stack = (void*)malloc(sizeof(char)*STACKSIZE);
	void* stack_size = stack + STACKSIZE - 1;
	int pid = clone(execute_command, stack_size, SIGCHLD, arg);

	if (pid == -1) {
		printf("Clone failed!\n");
		exit(1);
	}

	waitpid(pid, NULL, 0);
	free(stack);
	return 0;
}


void parent(int argc, char* argv[]) {
	char** args = create_exec_arguments(argc, argv);
	Command command = {argv[2], args};

	void* stack = (void*)malloc(sizeof(char)*STACKSIZE);
	void* stack_size = stack + STACKSIZE - 1;
	int pid = clone(child, stack_size, SIGCHLD | CLONE_NEWUTS, (void*)&command);

	if (pid == -1) {
		printf("Clone failed!\n");
		exit(1);
    }

	waitpid(pid, NULL, 0);
	free(args);
	free(stack);
}


int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Usage: micro-docker [option] [executable and parameters]\n");
        return 0;
    }

    if (!strcmp(argv[1], "run")) {
        parent(argc, argv);
    } else {
        printf("Option does not exist!\n");
    }

    return 0;
}
