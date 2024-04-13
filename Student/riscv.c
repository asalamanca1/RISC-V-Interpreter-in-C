#include <errno.h>   //errno
#include <stdint.h>  //uint32_t
#include <stdio.h>   //printf() fprintf()
#include <stdlib.h>  //malloc() & free()
#include <string.h>  //strerror()

#include "my_string.h"
#include "process_file.h"
#include "tokenizer.h"

#define N_REGISTERS ((size_t)32)
#define MEM_SIZE ((size_t)(1 << 10))

uint32_t r[N_REGISTERS];
int32_t pc;
unsigned char mem[MEM_SIZE];

static void error_no_memory(void) {
  fprintf(stderr, "No more memory available. Error: %s\n", strerror(errno));
}

void init_memory_elements(void) {
  // Initialize registers
  for (size_t i = 0; i < N_REGISTERS; i++) r[i] = ((uint32_t)0);

  // Initialize PC to 0
  pc = ((int32_t)0);

  // Initialize all memory to 0
  for (size_t i = 0; i < MEM_SIZE; ++i) mem[i] = ((unsigned char)0);

  // Set sp to be the top part of the memory
  r[2] = MEM_SIZE;
}

static int print_registers(char *fd_name) {
  FILE *fptr;

  if ((fptr = fopen(fd_name, "w")) == NULL) return 1;

  // Print all registers
  fprintf(fptr, "Registers:\n");
  for (size_t i = 0; i < N_REGISTERS; i++)
    fprintf(fptr, "X[%zu] = %u\n", i, r[i]);

  return 0;
}

/**
 * Fill out this function and use it to read interpret user input to execute
 * RV32 instructions. You may expect that a single, properly formatted RISC-V
 * instruction string will be passed as a parameter to this function.
 */
int interpret(char *instr) {
  // TODO:

  return 1;
}

int main(int argc, char **argv) {
  FILE *file;
  char *buffer;

  if (argc != 3) {
    fprintf(stderr,
            "Only three parameters must be passed where argv[1] = filename "
            "with the assembly instructions and argv[2] = filename to save the "
            "registers status.\n");
    return 1;
  }

  if (process_file(argv[1])) return 1;
  if (open_file()) return 1;

  // Initialize PC, registers and memory
  init_memory_elements();

  buffer = (char *)malloc((LINE_SIZE + 1) * sizeof(char));

  if (buffer == NULL) {
    close_file();
    return 1;
  }

  /* --- Your code starts here. --- */

  // For each line in the file, interpret it.
  // NOTE: Use get_line() function in process_file.h

  /* --- Your code ends here. --- */

  close_file();
  free(buffer);

  return print_registers(argv[2]);
}
