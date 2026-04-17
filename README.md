# Linux Mini Debugger

A simple debugger built using ptrace on Linux (ARM64).

## Features
- Read registers (PC, SP, X registers)
- Read memory
- Set breakpoint using BRK instruction
- Single step execution
- Continue execution

## Commands
- regs → show registers
- step → execute one instruction
- continue → resume program

## Build
make

## Run
./debugger ./test
