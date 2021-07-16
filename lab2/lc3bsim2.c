/*
    Name 1: Sidharth Nair
    UTEID 1: sn25377
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

enum {
    BR, ADD, LDB, STB, JSR_R, AND, LDW, STW, RTI, XOR, UNUSED1, UNUSED2, JMP, SHF, LEA, TRAP
};

enum {
    UNSIGNED, SIGNED
};

int power(int a, int b) {
    if (a == 0) {
        return 0;
    }
    int ret = 1;
    for (int i = 0; i < b; i++) {
        ret *= a;
    }
    return ret;
}

/*
int sext(int num, int bits) {
    if (num & power(2, bits - 1)) {
        return (num | (0xFFFF - (power(2, bits) - 1)));
    }
    else {
        return num;
    }
}
 */
int bitsToDec(int num, int highestBit, int lowestBit, int isSigned) {
    if (!isSigned) {
        return ((num & (power(2, highestBit + 1) - 1 - (power(2, lowestBit) - 1))) >> lowestBit);
    }
    else {
        int ret = ((num & (power(2, highestBit) - 1 - (power(2, lowestBit) - 1))) >> lowestBit);
        return (num & power(2, highestBit)) ? -1 * power(2, highestBit) + ret : ret;
    }
}

// Updates condition codes based on value stored in register reg
void updateCC(int reg) {
    NEXT_LATCHES.N = 0; NEXT_LATCHES.Z = 0; NEXT_LATCHES.P = 0;
    if (bitsToDec(NEXT_LATCHES.REGS[reg], 15, 0, SIGNED) < 0) {
        NEXT_LATCHES.N = 1;
    }
    else if (bitsToDec(NEXT_LATCHES.REGS[reg], 15, 0, SIGNED) == 0) {
        NEXT_LATCHES.Z = 1;
    }
    else {
        NEXT_LATCHES.P = 1;
    }
}

#define LowByte(x) ((x) & 0x00FF)
#define HighByte(x) ((x) & 0xFF00)

void process_instruction() {
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */
      int IR = Low16bits((MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8) + MEMORY[CURRENT_LATCHES.PC >> 1][0]);
      int DR = bitsToDec(IR, 11, 9, UNSIGNED);
      int SR1 = bitsToDec(IR, 8, 6, UNSIGNED);
      int SR = SR1;
      int BaseR = SR1;
      int SR2 = bitsToDec(IR, 2, 0, UNSIGNED);
      int A = bitsToDec(IR, 5, 5, UNSIGNED);
      int D = bitsToDec(IR, 4, 4, UNSIGNED);
      int IMM5 = bitsToDec(IR, 4, 0, SIGNED);
      int AMT4 = bitsToDec(IR, 3, 0, UNSIGNED);
      int PCO9 = bitsToDec(IR, 8, 0, SIGNED);
      int PCO11 = bitsToDec(IR, 10, 0, SIGNED);
      int O6 = bitsToDec(IR, 5, 0, SIGNED);
      int TRPV8 = bitsToDec(IR, 7, 0, UNSIGNED);

      int OPCODE = bitsToDec(IR, 15, 12, UNSIGNED);
      NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
      switch(OPCODE) {
          case BR: {
              int BEN = CURRENT_LATCHES.N & (bitsToDec(IR, 11, 11, UNSIGNED)) ||
                        CURRENT_LATCHES.Z & (bitsToDec(IR, 10, 10, UNSIGNED)) ||
                        CURRENT_LATCHES.P & (bitsToDec(IR, 9, 9, UNSIGNED));
              if (BEN) {
                  NEXT_LATCHES.PC = NEXT_LATCHES.PC + (PCO9 << 1);
              }
              break;
          }
          case ADD: {
              if (A) {
                  NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + IMM5);
              }
              else {
                  NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + CURRENT_LATCHES.REGS[SR2]);
              }
              updateCC(DR);
              break;
          }
          case LDB: {
              int address = CURRENT_LATCHES.REGS[BaseR] + O6;
              NEXT_LATCHES.REGS[DR] = Low16bits(bitsToDec(MEMORY[address >> 1][address & 0x1], 7, 0, SIGNED));
              updateCC(DR);
              break;
          }
          case STB: {
              int address = CURRENT_LATCHES.REGS[BaseR] + O6;
              MEMORY[address >> 1][address & 0x1] = LowByte(CURRENT_LATCHES.REGS[DR]);
              break;
          }
          case JSR_R: {
              int TEMP = NEXT_LATCHES.PC;
              if (bitsToDec(IR, 11, 11, UNSIGNED)) {
                  NEXT_LATCHES.PC = NEXT_LATCHES.PC + (PCO11 << 1);
              }
              else {
                  NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BaseR];
              }
              NEXT_LATCHES.REGS[7] = TEMP;
              break;
          }
          case AND: {
              if (A) {
                  NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & IMM5);
              }
              else {
                  NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & CURRENT_LATCHES.REGS[SR2]);
              }
              updateCC(DR);
              break;
          }
          case LDW: {
              int address = CURRENT_LATCHES.REGS[BaseR] + (O6 << 1);
              NEXT_LATCHES.REGS[DR] = Low16bits(HighByte(MEMORY[address >> 1][1] << 8) + LowByte(MEMORY[address >> 1][0]));
              updateCC(DR);
              break;
          }
          case STW: {
              int address = CURRENT_LATCHES.REGS[BaseR] + (O6 << 1);
              MEMORY[address >> 1][1] = HighByte(CURRENT_LATCHES.REGS[DR]) >> 8;
              MEMORY[address >> 1][0] = LowByte(CURRENT_LATCHES.REGS[DR]);
              break;
          }
          case RTI: { // DOES NOT NEED TO BE IMPLEMENTED
              break;
          }
          case XOR: {
              if (A) {
                  NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ IMM5);
              }
              else {
                  NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ CURRENT_LATCHES.REGS[SR2]);
              }
              updateCC(DR);
              break;
          }
          case JMP: {
              NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BaseR];
              break;
          }
          case SHF: {
              if (A) { // RSHFA
                  if (bitsToDec(CURRENT_LATCHES.REGS[SR], 15, 15, UNSIGNED)) {
                      int TEMP = CURRENT_LATCHES.REGS[SR];
                      for (int i = AMT4; i > 0; i--) {
                          TEMP = (TEMP >> 1) + 0x8000;
                      }
                      NEXT_LATCHES.REGS[DR] = Low16bits(TEMP);
                  }
                  else {
                      NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] >> AMT4);
                  }
              }
              else if (D) { // RSHFL
                  NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] >> AMT4);
              }
              else { // LSHF
                  NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] << AMT4);
              }
              updateCC(DR);
              break;
          }
          case LEA: {
              NEXT_LATCHES.REGS[DR] = Low16bits(NEXT_LATCHES.PC + (PCO9 << 1));
              break;
          }
          case TRAP: {
              NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
              NEXT_LATCHES.PC = Low16bits(HighByte(MEMORY[TRPV8][1] << 8) + LowByte(MEMORY[TRPV8][0]));
              break;
          }

      }
}
