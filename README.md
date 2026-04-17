🧠 Linux Mini Debugger (ARM64)

A custom interactive debugger built in C using the "ptrace()" system call.

---

🚀 Features

- 🔍 Register inspection (PC, SP, general registers)
- 🧠 Memory reading at runtime
- 🛑 Breakpoint handling (ARM64 BRK instruction)
- ▶️ Instruction-level stepping
- 💻 Interactive CLI (regs, step, continue, exit)

---

🛠️ Tech Stack

- C Programming
- Linux System Calls
- ptrace API
- ARM64 Architecture

---

▶️ Usage

make
gcc test.c -o test
./debugger ./test

---

💻 Commands

Command| Description
regs| Show registers
step| Execute one instruction
continue| Resume execution
exit| Quit debugger

---

📸 Example Output

(debugger) regs
PC: ffff...

(debugger) step
PC: ffff...

(debugger) continue
HELLO DEBUGGER!

---

📚 What I Learned

- Low-level process control using ptrace
- Debugging internals and CPU execution flow
- Memory and register inspection
- Building CLI tools in C

---


