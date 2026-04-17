#include <string.h>
#include "debugger.h"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <elf.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void start_debugger(char *program) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(program, program, NULL);
        perror("execl failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);

        printf("[Debugger] Process started. PID: %d\n", pid);
        printf("[Debugger] Child stopped.\n");

        // ===== ARM64 Registers =====
        struct user_regs_struct regs;
        struct iovec iov;

        iov.iov_base = &regs;
        iov.iov_len = sizeof(regs);

        if (ptrace(PTRACE_GETREGSET, pid, (void*)NT_PRSTATUS, &iov) == -1) {
            perror("ptrace GETREGSET failed");
            return;
        }

        printf("\n[Registers]\n");
        printf("PC: %llx\n", regs.pc);
        printf("SP: %llx\n", regs.sp);

        for (int i = 0; i < 5; i++) {
            printf("X[%d]: %llx\n", i, regs.regs[i]);
        }

	printf("\n[Memory]\n");

	// Read memory at current PC
	long data = ptrace(PTRACE_PEEKDATA, pid, (void*)regs.pc, NULL);

	printf("Memory at PC (0x%llx): 0x%lx\n", regs.pc, data);

	printf("\n[Stepping]\n");
	
	// ===== Breakpoint =====
	printf("\n[Breakpoint]\n");

	unsigned long addr = regs.pc;
	char command[50];

	while (1) {
	    printf("\n(debugger) ");
	    scanf("%s", command);

	    if (strcmp(command, "step") == 0) {
	        ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
	        waitpid(pid, &status, 0);

	        ptrace(PTRACE_GETREGSET, pid, (void*)NT_PRSTATUS, &iov);
	        printf("PC: %llx\n", regs.pc);
	    }
	    else if (strcmp(command, "continue") == 0) {
	        ptrace(PTRACE_CONT, pid, NULL, NULL);
	        waitpid(pid, &status, 0);
	        printf("Process continued\n");
	    }
	    else if (strcmp(command, "regs") == 0) {
	        ptrace(PTRACE_GETREGSET, pid, (void*)NT_PRSTATUS, &iov);
	        printf("PC: %llx\n", regs.pc);
	    }
	    else if (strcmp(command, "exit") == 0) {
	        break;
	    }
	    else {
	        printf("Unknown command\n");
	    }
	}

	// Save original instruction
	long original = ptrace(PTRACE_PEEKDATA, pid, (void*)addr, NULL);

	// Insert BRK instruction (ARM64)
	long breakpoint = (original & ~0xFFFFFFFF) | 0xD4200000;

	ptrace(PTRACE_POKEDATA, pid, (void*)addr, (void*)breakpoint);

	printf("Breakpoint set at: 0x%lx\n", addr);
	
	ptrace(PTRACE_CONT, pid, NULL, NULL);
	waitpid(pid, &status, 0);
	
	printf("Breakpoint hit!\n");
	// ===== Restore original instruction =====
	ptrace(PTRACE_POKEDATA, pid, (void*)addr, (void*)original);

	// ===== Fix PC =====
	regs.pc = addr;
	ptrace(PTRACE_SETREGSET, pid, (void*)NT_PRSTATUS, &iov);

	printf("Restored original instruction\n");

	// Step 1 instruction
	ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
	waitpid(pid, &status, 0);

	printf("Stepped one instruction!\n");
	
	ptrace(PTRACE_GETREGSET, pid, (void*)NT_PRSTATUS, &iov);

	printf("\n[After Step]\n");
	printf("PC: %llx\n", regs.pc);

        // Continue execution
        ptrace(PTRACE_CONT, pid, NULL, NULL);
        waitpid(pid, &status, 0);

        printf("[Debugger] Process exited.\n");
    }
}
