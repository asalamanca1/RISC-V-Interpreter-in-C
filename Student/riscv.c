
#include "my_string.h"
#include "process_file.h"
#include "tokenizer.h"
#include <errno.h>  //errno
#include <stdint.h> //uint32_t
#include <stdio.h>  //printf() fprintf()
#include <stdlib.h> //malloc() & free()
#include <string.h> //strerror()
#define N_REGISTERS ((size_t)32)
#define MEM_SIZE ((size_t)(1 << 10))
#include <inttypes.h> // for PRIu32
uint32_t r[N_REGISTERS];
int32_t pc;
unsigned char mem[MEM_SIZE];
int isJump=0;
static void error_no_memory(void) {
  fprintf(stderr, "No more memory available. Error: %s\n", strerror(errno));
}
void init_memory_elements(void) {
  // Initialize registers
  for (size_t i = 0; i < N_REGISTERS; i++)
    r[i] = ((uint32_t)0);
  // Initialize PC to 0
  pc = ((int32_t)0);
  // Initialize all memory to 0
  for (size_t i = 0; i < MEM_SIZE; ++i)
    mem[i] = ((unsigned char)0);
  // Set sp to be the top part of the memory
  r[2] = MEM_SIZE;
}
static int print_registers(char *fd_name) {
  FILE *fptr;
  if ((fptr = fopen(fd_name, "w")) == NULL) {
    return 1;
  }
  fprintf(fptr, "Registers:\n");
  for (size_t i = 0; i < N_REGISTERS; i++) {
    fprintf(fptr, "X[%zu] = %u\n", i, r[i]);
  }
  return 0;
}
/**
 * Fill out this function and use it to read interpret user input to execute
 * RV32 instructions. You may expect that a single, properly formatted RISC-V
 * instruction string will be passed as a parameter to this function.
 */
//TO DO: sign extend save/load (for byte and words), implement different formats with and without parenthesis for save/load
int isJumpInstr(int jump){
  return isJump=jump;
}
// helper function to get register number of psuedo-register
int getRegister(char *pseudoRegister) {
  // store psuedonames of registers where each index corresponds to the number
  // of register, ex:sp-->x2, a0-->x10, etc.
  const char *validPseudoRegisters[] = {
      "ZERO", "RA", "SP", "GP", "TP",  "T0",  "T1", "T2", "S0", "S1", "A0",
      "A1",   "A2", "A3", "A4", "A5",  "A6",  "A7", "S2", "S3", "S4", "S5",
      "S6",   "S7", "S8", "S9", "S10", "S11", "T3", "T4", "T5", "T6"};
  // loop through validRegisters array to find the index of the register
  for (int i = 0; i < 32; i++) {
    if (strcmp(pseudoRegister, validPseudoRegisters[i]) == 0) {
      return i;
    }
  }
  const char *validRegisters[] = {
      "X0",  "X1",  "X2",  "X3",  "X4",  "X5",  "X6",  "X7",
      "X8",  "X9",  "X10", "X11", "X12", "X13", "X14", "X15",
      "X16", "X17", "X18", "X19", "X20", "X21", "X22", "X23",
      "X24", "X25", "X26", "X27", "X28", "X29", "X30", "X31"};
  for (int i = 0; i < 32; i++) {
    if (strcmp(pseudoRegister, validRegisters[i]) == 0) {
      return i;
    }
  }
  if (strcmp(pseudoRegister, "FP") == 0) {
      return 8;
  }
  // register not found
  return -1;
}
//int our_atoi(const char *nptr) { //deleted
int our_atoi(char *nptr) { //added
  int result =
      0; // Initialize the result variable to hold the converted integer
  int negative = 0; // Flag to track if the number is negative
  // check if there are whitespaces
  while (*nptr == ' ' || (*nptr >= '\t' && *nptr <= '\r'))
    nptr++; // Skip whitespace characters
  // check sign
  if (*nptr == '-') { // If the first character is '-', the number is negative
 
    negative = 1;     // for testing
    nptr++;           // Move to the next character
  } else if (*nptr == '+') { // If the first character is '+', just move to the
                             // next character
    nptr++;                  // Move to the next character
  }
  // Convert digits to integer
  while (*nptr >= '0' &&
         *nptr <= '9') { // While the current character is a digit
    result =
        result * 10 + (*nptr - '0'); // Convert the digit character to its
                                     // integer value and add it to the result
    nptr++;                          // Move to the next character
  }
  // Apply sign if negative
  if (negative == 1)
 
    result = -result; // If the number is negative, make the result negative
  return result;      // Return the final converted integer
}
void overrideX0(void) {
  r[0] = (uint32_t)0;
  return;
}
/**
-----------------------------------------------------------------------------------------------
 * Fill out this function and use it to read interpret user input to execute
 * RV32 operations. You may expect that a single, properly formatted RISC-V
 * operation string will be passed as a parameter to this function.
 */
int interpret(char *instr) {
  printf("CALLING INTERPRET FUNCTION FOR INSTR: %s\n",instr);
  if (strcmp(instr, "") == 0) {
    return close_file();
  }
  
  char **instruction = tokenize(instr, " ");
  if (instruction == NULL) {
    free(instruction);
    return close_file();
  }
  printf("Tokenized instr into 'instruction' variable:\ninstruction[0]: %s, instruction[1]: %s\n",instruction[0], instruction[1]);
  
  
  char *operation = instruction[0];
  if (operation == NULL) {
    free(instruction);
    printf("Operation is null after calling tokenize, we will now free instruction.\n");
    error_no_memory();
    return close_file();
  }
  char *params = instruction[1];
  char **operands = tokenize(params, ",");
  printf("Tokenized instruction[1] into 'operands' variable:\noperands[0]: %s, operands[1]: %s, operands[2]: %s\n",operands[0], operands[1],operands[2]);
  if (operands == NULL) {
    printf("Operands returns null after tokenizing, we will now free operation, instruction, and operands\n");
    error_no_memory();
    free(instruction);
    free(operands);
    return close_file();
  }
// LOADS:
  // LB operation
  if (str_cmp(operation, "LB") == 0) {
    printf("This instruction is in its simplest form.\n");
    if(str_chr(operands[1], '(')==NULL){
      char *rd = operands[0];
      char *rs1 = operands[1];
      char *imm = operands[2];
    
      // Get one byte of memory at a time and place it in num
      uint32_t load = 0;
      load |= ((uint32_t)(mem[(int32_t)our_atoi(imm)] ));
      printf("The memory location we are loading from is mem[%s+%s]\n",rs1,imm);
      if (load & 0x80) {
        printf("We are sign extending the byte since it is negative\n");
        // If the most significant bit is set, perform sign extension
        load |= 0xFFFFFF00; // Set all bits above bit 7
        r[getRegister(rd)] = (int32_t)load;
        printf("The value we are loading from mem[] is %d\n", load);
      }
      else{
        printf("This byte is not sign extended\n");
        // Assign the sign-extended value to the register
        r[getRegister(rd)] = (uint32_t)load;
      }
      // free memory
      free(operands);
      free(instruction);
    }
    else{
      char *rd = operands[0];
      char *temp = operands[1];
      char **tempList = tokenize(temp, "(");
      if (tempList == NULL) {
        free(tempList);
        free(operands);
        free(instruction);
        
        error_no_memory();
        return close_file();
      }
      char *delete = str_p_brk(tempList[1], ")");
      *delete = '\0';
      char *imm = tempList[0];
      char *rs1 = tempList[1];
      
      // Get one byte of memory at a time and place it in num
      uint32_t load = 0;
      load |= ((uint32_t)(mem[(int32_t)our_atoi(imm)] ));
      
  
      if (load & 0x80) {
      
        // If the most significant bit is set, perform sign extension
        load |= 0xFFFFFF00; // Set all bits above bit 7
        r[getRegister(rd)] = (int32_t)load;
      }
      else{
        // Assign the sign-extended value to the register
        r[getRegister(rd)] = (uint32_t)load;
      }
      // override x0 if it was modified
      if (getRegister(rd) == 0) {
        overrideX0();
      }
      // free memory
      free(tempList);
      free(operands);
      free(instruction);
      
    }
    
    
    // override x0 if it was modified
    overrideX0();
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // LW operation
  else if (str_cmp(operation, "LW") == 0) {
    if(str_chr(operands[1], '(')==NULL){
      char *rd = operands[0];
      char *rs1 = operands[1];
      char *imm = operands[2];
      printf("LOAD BYTE");
      printf("%s\n", imm);
      printf("%s\n", rs1);
      printf("%s\n", rd);
      
      // Get one byte of memory at a time and place it in num
      uint32_t load = 0;
      if (getRegister(rs1) == 2) {
       
        
        load |= ((uint32_t)(mem[(int32_t)r[our_atoi(rs1)] + (int32_t)our_atoi(imm)] ));
        load |= ((uint32_t)(mem[(int32_t)r[our_atoi(rs1)] + (int32_t)our_atoi(imm)+(int32_t)1] <<8));
        load |= ((uint32_t)(mem[(int32_t)r[our_atoi(rs1)] + (int32_t)our_atoi(imm)+(int32_t)2] <<16));
        load |= ((uint32_t)(mem[(int32_t)r[our_atoi(rs1)] + (int32_t)our_atoi(imm)+(int32_t)3] <<24));
        r[getRegister(rd)] = (uint32_t)load;
      } else {
        r[getRegister(rd)] = r[our_atoi(rs1) + our_atoi(imm)];
      }
      
      // free memory
      free(operands);
      free(instruction);
      
    }
    else{
      char *rd = operands[0];
      char *temp = operands[1];
      char **tempList = tokenize(temp, "(");
      if (tempList == NULL) {
        free(tempList);
        free(operands);
        free(instruction);
        
        error_no_memory();
        return close_file();
      }
      char *delete = str_p_brk(tempList[1], ")");
      *delete = '\0';
      char *imm = tempList[0];
      char *rs1 = tempList[1];

      uint32_t load = 0;
      load |= ((uint32_t)(mem[(int32_t)r[our_atoi(rs1)] + (int32_t)our_atoi(imm)] ));
      load |= ((uint32_t)(mem[(int32_t)r[our_atoi(rs1)] + (int32_t)our_atoi(imm)+(int32_t)1] <<8));
      load |= ((uint32_t)(mem[(int32_t)r[our_atoi(rs1)] + (int32_t)our_atoi(imm)+(int32_t)2] <<16));
      load |= ((uint32_t)(mem[(int32_t)r[our_atoi(rs1)] + (int32_t)our_atoi(imm)+(int32_t)3] <<24));
      r[getRegister(rd)] = (uint32_t)load;
     
      // override x0 if it was modified
      if (getRegister(rd) == 0) {
        overrideX0();
      }
      // free memory
      free(tempList);
      free(operands);
      free(instruction);
      
    }
    overrideX0();
    isJumpInstr(0);
    return 1;
    
  }
// STORES:
  // SB operation
  else if (str_cmp(operation, "SB") == 0) {
    printf("STORE BYTE\n");


    if(str_chr(operands[1], '(')==NULL){
      char *rd = operands[0];
      char *rs1 = operands[1];
      char *imm = operands[2];
      
      mem[(int32_t)our_atoi(imm)] = (unsigned char)(r[getRegister(rs1)] & 0xFF);
      // free memory
      free(operands);
      free(instruction);
      
    }
    else{
      char *rd = operands[0];
      char *temp = operands[1];
      char **tempList = tokenize(temp, "(");
      if (tempList == NULL) {
        free(tempList);
        free(operands);
        free(instruction);
        
        error_no_memory();
        return close_file();
      }
      char *delete = str_p_brk(tempList[1], ")");
      *delete = '\0';
      char *imm = tempList[0];
      char *rs1 = tempList[1];
      
      // if rd is stack pointer, store word in heap memory
      if (getRegister(rs1) == 2) {
        mem[our_atoi(rs1) + our_atoi(imm)] = (unsigned char)(r[getRegister(rd)] & 0xFF);
      

      }
      // if rd isnt stack pointer, store word in register
      else {
        r[getRegister(rs1)] = r[getRegister(rd)];
       
      }
      // free memory
      free(tempList);
      free(operands);
      free(instruction);
      


    }
    // override x0 if it was modified
    overrideX0();
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // SW operation
  else if (str_cmp(operation, "SW") == 0) {

    if(str_chr(operands[1], '(')==NULL){
      char *rd = operands[0];
      char *rs1 = operands[1];
      char *imm = operands[2];
      
      mem[(int32_t)our_atoi(imm)] = (unsigned char)(r[getRegister(rs1)] & 0xFF);
      mem[(int32_t)our_atoi(imm) + (int32_t)1] = (unsigned char)((r[getRegister(rs1)] >> 8) & 0xFF);
      mem[(int32_t)our_atoi(imm) + (int32_t)2] = (unsigned char)((r[getRegister(rs1)] >> 16) & 0xFF);
      mem[(int32_t)our_atoi(imm) + (int32_t)3] = (unsigned char)((r[getRegister(rs1)] >> 24) & 0xFF);
  
     
      // free memory
      free(operands);
      free(instruction);
      

    }
    else{
      char *rd = operands[0];
      char *temp = operands[1];
      char **tempList = tokenize(temp, "(");
      if (tempList == NULL) {
        free(tempList);
        free(operands);
        free(instruction);
        
        error_no_memory();
        return close_file();
      }
      char *delete = str_p_brk(tempList[1], ")");
      *delete = '\0';
      char *imm = tempList[0];
      char *rs1 = tempList[1];
      
      mem[(int32_t)our_atoi(imm)] = (unsigned char)(r[getRegister(rs1)] & 0xFF);
      mem[(int32_t)our_atoi(imm) + (int32_t)1] = (unsigned char)((r[getRegister(rs1)] >> 8) & 0xFF);
      mem[(int32_t)our_atoi(imm) + (int32_t)2] = (unsigned char)((r[getRegister(rs1)] >> 16) & 0xFF);
      mem[(int32_t)our_atoi(imm) + (int32_t)3] = (unsigned char)((r[getRegister(rs1)] >> 24) & 0xFF);
    
     
      // free memory
      free(tempList);//third tokenize
      free(operands);//second tokenize
      free(instruction);//first tokenize
      
    }
    
    // override x0 if it was modified
    overrideX0();
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // ARITHMETIC:
  // ADD operation
  else if (str_cmp(operation, "ADD") == 0) {
    printf("ADD\n");
    char *rd = operands[0];
    char *rs1 = operands[1];
    char *rs2 = operands[2];
    // RD <- RS1 + RS2
    r[getRegister(rd)] = r[getRegister(rs1)] + r[getRegister(rs2)];
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // ADDI operation
  else if (str_cmp(operation, "ADDI") == 0) {
    printf("ADDI\n");
    char *rd = operands[0];
    char *rs1 = operands[1];
    char *imm = operands[2];
    // RD -> RS1 + IMM
    r[getRegister(rd)] = r[getRegister(rs1)] + (u_int32_t)our_atoi(imm);
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // SUB operation
  else if (str_cmp(operation, "SUB") == 0) {
    printf("SUB\n");
    char *rd = operands[0];
    char *rs1 = operands[1];
    char *rs2 = operands[2];
    // RD -> RS1 - RS2
    r[getRegister(rd)] = r[getRegister(rs1)] - r[getRegister(rs2)];
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);

    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // BITWISE OPERATORS:
  //  XOR operation
  else if (str_cmp(operation, "XOR") == 0) {
    printf("XOR\n");
    char *rd = operands[0];
    char *rs1 = operands[1];
    char *rs2 = operands[2];
    // RD -> RS1 ^ RS2
    r[getRegister(rd)] = r[getRegister(rs1)] ^ r[getRegister(rs2)];
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // XORI operation
  else if (str_cmp(operation, "XORI") == 0) {
    printf("XORI\n");
    char *rd = operands[0];
    char *rs1 = operands[1];
    char *imm = operands[2];
    // RD -> RS1 ^ IMM
    r[getRegister(rd)] = r[getRegister(rs1)] ^ (u_int32_t)our_atoi(imm);
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // SLLI operation
  else if (str_cmp(operation, "SLLI") == 0) {
    printf("SLLI\n");
    char *rd = operands[0];
    char *rs1 = operands[1];
    char *imm = operands[2];
    // RD -> RS1 << IMM
    r[getRegister(rd)] = r[getRegister(rs1)] << our_atoi(imm);
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // SRLI operation
  else if (str_cmp(operation, "SRLI") == 0) {
    printf("SRLI\n");
    char *rd = operands[0];
    char *rs1 = operands[1];
    char *imm = operands[2];
    // RD -> RS1 >> IMM
    r[getRegister(rd)] = r[getRegister(rs1)] >> our_atoi(imm);
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // Pseudo operations:
  // MV operation
  else if (str_cmp(operation, "MV") == 0) {
    char *rd = operands[0];
    char *rs = operands[1];
    r[getRegister(rd)] = r[getRegister(rs)];
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // LI operation
  else if (str_cmp(operation, "LI") == 0) {
    char *rd = operands[0];
    char *imm = operands[1];
    r[getRegister(rd)] = (uint32_t)(our_atoi(imm));
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // NED operation
  else if (str_cmp(operation, "NEG") == 0) {
    char *rd = operands[0];
    char *rs = operands[1];
    r[getRegister(rd)] = ~(r[getRegister(rs)]) + (uint32_t)(1);
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // NOT operation
  else if (str_cmp(operation, "NOT") == 0) {
    char *rd = operands[0]; // Destination register
    char *rs = operands[1]; // Source register
    r[getRegister(rd)] = ~(r[getRegister(rs)]);
    // override x0 if it was modified
    if (getRegister(rd) == 0) {
      overrideX0();
    }
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to False
    isJumpInstr(0);
    return 1;
  }
  // JUMP OFFSET
  // JAL operation
  else if (str_cmp(operation, "JAL") == 0) {
    printf("JAL\n");
    char *rd = operands[0];
    char *imm = operands[1];
    r[getRegister(rd)] = (uint32_t)(pc);
    r[getRegister(rd)] += (uint32_t)(4);
    printf("value at RA is: %u\n", r[getRegister(rd)]);
    pc = pc + (int32_t)(our_atoi(imm) );
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to True
    isJumpInstr(1);
    //override x0 to 0
    overrideX0();
    return 1;
  }
  // J operation
  else if (str_cmp(operation, "J") == 0) {
    printf("J\n");
    
    char *offset = operands[0];
    printf("%s\n", offset);
    int intOffset = our_atoi(offset);
    
    int32_t castedOffset = (int32_t)(intOffset);
    //printf("atoi offset: %d\n", intOffset);
    //printf("The int32_t offset: %d\n", castedOffset);
    //pc = pc + (int32_t)(our_atoi(offset) / 4);
    //printf("pc before increasing jump: %zu\n", pc);
    pc += castedOffset;
    //printf("pc after increasing jump: %d\n", pc);
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to True
    isJumpInstr(1);
    //override x0 to 0
    overrideX0();
    return 1;
  }
  // JUMP OFFSET + REG
  //  JALR operation
  else if (str_cmp(operation, "JALR") == 0) {
    printf("JALR\n");
    char *rd = operands[0];
    char *rs1 = operands[1];
    char *imm = operands[2];
    r[getRegister(rd)] = (uint32_t)(pc + (int32_t)4);
    overrideX0();
    pc = (int32_t)(r[getRegister(rs1)]) + (int32_t)(our_atoi(imm));
    //printf("jalr pc: %zu\n", pc);
    // free memory
    free(operands);
    free(instruction);
    
    //set isJump to True
    isJumpInstr(1);
    //override x0 to 0
    overrideX0();
    return 1;
  }
  // JR operation
  else if (str_cmp(operation, "JR") == 0) {
    printf("JR\n");
    char *rs = operands[0];
    r[0] = (uint32_t)(pc + (int32_t)4);
    //override x0 to 0
    overrideX0();
    pc = (int32_t)(r[getRegister(rs)] + (u_int32_t)0);
    // free memory
    free(operands);
    free(instruction);
    
    //override x0 to 0
    overrideX0();
    //set isJump to True
    isJumpInstr(1);
    return 1;
  }
  // free memory
  free(operands);
  free(instruction);
  
  // return failure case if no operation was found
  close_file();
  return 0;
}
int main(int argc, char **argv) {
  // FILE *file;
  char *buffer;
  if (argc != 3) {
    fprintf(stderr,
            "Only two parameters must be passed where the second parameter is "
            "the filename with the assembly instructions.\n");
    return 1;
  }
  if (process_file(argv[1]))
    return 1;
  if (open_file())
    return 1;
  // Initialize PC, registers and memory
  init_memory_elements();
  buffer = (char *)malloc((LINE_SIZE + 1) * sizeof(char));
  if (buffer == NULL) {
    free(buffer);
    close_file();
    return 1;
  }
  /* --- Your code starts here. --- */
  // For each line in the file, interpret it.
  // NOTE: Use get_line() function in process_file.h
  
  //printf("pc before first get line: %zu\n", pc);
  //get_line(buffer, (size_t)(pc/4));
  //printf("pc after first get line: %zu\n", pc);
  //interpret(buffer);
  //printf("pc after first interpret: %zu\n", pc);
  //pc+=4;
  while (pc > (int32_t)(-1)) {
    printf("pc before iteration: %d\n", pc);
    get_line(buffer, (size_t)(pc/4));
    interpret(buffer);
    
    //we must figure out a way to choose between pc+4 or jump instruction pc override. if its a jump instr, we will not add 4 to pc
    if(!isJump){
      pc+=4;
    }
    printf("pc after iteration: %d\n", pc);
    
    
  }
  //pc+=4;
  printf("pc after while loop: %d\n", pc);
  /* --- Your code ends here. --- */
  
  free(buffer);
  close_file();
  return print_registers(argv[2]);
}
