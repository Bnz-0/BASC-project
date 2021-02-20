#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>


int main(int argc, char** argv, char** envp) {
	pid_t pid;

	if (argc < 2) {
		puts("Usage: instruction_counter path_to_executable [args...]");
		exit(0);
	}

	if ((pid = fork()) == 0) {
		if (ptrace(PTRACE_TRACEME, 42, 0, 0)) {
			perror("PTRACE_TRACEME");
			exit(1);
		}
		execve(argv[1], &argv[1], envp);
	}

	int status;
	if (waitpid(pid, &status, 0) == -1) {
		perror("waitpid");
		exit(1);
	}
	else if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
		printf("Tracing %s (%jd)\n", argv[1], (intmax_t)pid);

	unsigned long n_instructions = 0;
	do {
		if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) || waitpid(pid, &status, 0) == -1) {
			perror("PTRACE_SINGLESTEP");
			exit(1);
		}
		n_instructions++;
	} while(!WIFEXITED(status) && !WIFSIGNALED(status));

	printf("Child exited; n_instructions=%lu\n", n_instructions);
	return 0;
}

