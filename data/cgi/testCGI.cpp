#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

int main(int argc, char *argv[], char *envp[]) {
	char *args[] = {(char *)"./CGI.py", NULL};
	char *env[] = {(char *)"QUERY_STRING=name=Teddy&age=36", NULL};
	pid_t pid = fork();

	if (pid == -1) {
		std::cerr << "Error forking process." << std::endl;
		return 1;
	}
	else if (pid == 0) {
		execve(args[0], args, env);
		std::cerr << "Error executing Python script." << std::endl;
		return 1;
	}
	else {
		wait(NULL);
		std::cout << "Python script executed successfully." << std::endl;
		return 0;
	}
}