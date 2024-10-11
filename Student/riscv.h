#ifndef __RISCV_H__
#define __RISCV_H__






static void error_no_memory(void);
static int print_registers(char *fd_name);
void init_memory_elements(void);
void isJumpInstr(int jump);
int getRegister(char *pseudoRegister);
void overrideX0(void);
int interpret(char *instr);
int main(int argc, char **argv);

#endif /* RISCV_H */







