#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 64

// read the input from stdio
char *read_command() {
	char *buffer;
	int c, i = 0;

	// allocate size of buffer
	buffer = malloc(sizeof(char) * BUFFER_SIZE);
	if(!buffer) {
		fprintf(stderr, "Memory not available\n");
		exit(EXIT_FAILURE);
	}

	while(1) {
		c = getchar();
		buffer[i] = c;

		// check if input is end of line or newline
		if((c == EOF) || (c == '\n')) {
			buffer[i] = '\0';
			break;
		}
		i++;

		// increase size of buffer if it exceeds the current size
		if(i >= BUFFER_SIZE) {
			buffer = realloc(buffer, i + BUFFER_SIZE);
			if(!buffer) {
				fprintf(stderr, "Memory not available\n");
				free(buffer);
				exit(EXIT_FAILURE);
			}
		}
	}

	return buffer;
}

// split the command string into individual arguments
char **parse_command(char *command) {
	char **argument_list;
	int count = 0;
	int len = 0;

	// count number of words
	for(int i = 0; i < strlen(command); i++) {
		if((command[i] == ' ') || (command[i + 1] == '\0'))
			count++;
	}

	argument_list = malloc(sizeof(char *) * count);
	if(!argument_list) {
		fprintf(stderr, "Memory not available\n");
		free(argument_list);
		exit(EXIT_FAILURE);
	}

	// split the string at the given delimiter
	int i = 0;
	char *token = strtok(command, " ");

	while(token != NULL) {
		argument_list[i] = malloc(sizeof(char) * strlen(token));
		if(!argument_list[i]) {
			fprintf(stderr, "Memory not available\n");

			// free up memory
			for(int j = 0; j < i; j++)
				free(argument_list[j]);
			free(argument_list);

			exit(EXIT_FAILURE);
		}

		strcpy(argument_list[i], token);

		token = strtok(NULL, " ");
		i++;
	}

	return argument_list;
}

// execute command
void execute(char **args) {
	int status;
	pid_t pid = fork();

	// check for error in creating a fork
	if(pid == -1) {
		fprintf(stderr, "Unable to create a child process\n");
		return;
	}
	// child process
	else if(pid == 0) {
		execvp(args[0], args);
	}
	// parent process
	else {
		waitpid(pid, &status, WUNTRACED);
	}
}

int main() {
	char *command;
	char **args;

	while(1) {
		printf("$ ");

		command = read_command();
		if(!strcmp(command, "exit"))
			break;

		args = parse_command(command);
		execute(args);
	}

	return 0;
}