#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/user.h>
#include <capstone/capstone.h>

#define MAX_INSTRUCTION_BYTES 15
typedef unsigned long long addr_t;


addr_t get_rip(pid_t pid) {
	struct user_regs_struct regs;
	if (ptrace(PTRACE_GETREGS, pid, 0, &regs)) {
		perror("PTRACE_GETREGS");
		exit(1);
	}
	return regs.rip;
}

uint8_t peek_text(pid_t pid, addr_t addr) {
	static addr_t old_addr = 0; // because of page 0 is reserved, i'm pretty sure that this always works
	static long word;
	if (addr - old_addr >= sizeof(long)) {
		old_addr = addr;
		errno = 0;
		word = ptrace(PTRACE_PEEKTEXT, pid, addr, 0);
		if (errno) {
			perror("PTRACE_PEEKDATA");
			exit(1);
		}
	}
	return ((uint8_t*)&word)[addr - old_addr];
}


int main(int argc, char** argv, char** envp) {
	pid_t pid;

	if (argc < 2) {
		puts("Usage: dynamic_disasm path_to_executable [args...]");
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
	csh handle;
	cs_insn *insn;
	size_t count;
	uint8_t code_size;
	uint8_t code[MAX_INSTRUCTION_BYTES];
	if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
		fprintf(stderr, "ERROR: cs_open failed\n");
		exit(1);
	}
	do {
		code_size = 0;
		addr_t rip = get_rip(pid);
		do {
			code[code_size] = peek_text(pid, rip + code_size);
			count = cs_disasm(handle, code, ++code_size, rip, 0, &insn);
		} while (count < 1 && code_size < MAX_INSTRUCTION_BYTES);

		if(count < 1) {
			printf("ERROR: Failed to disassemble given code: ");	
			for (int i = 0; i < MAX_INSTRUCTION_BYTES; i++)
				printf("\\x%02X", code[i]);
			puts("");
		}

		for (size_t j = 0; j < count; j++) {
			printf("0x%"PRIx64":\t%s\t\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
		}
		cs_free(insn, count);

		if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) || waitpid(pid, &status, 0) == -1) {
			perror("PTRACE_SINGLESTEP");
			exit(1);
		}
		n_instructions++;

	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	cs_close(&handle);
	printf("Child exited; n_instructions=%lu\n", n_instructions);
	return 0;
}

