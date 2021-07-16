/*
        Name 1: Sidharth Nair
        UTEID 1: sn25377
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

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
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    LD_Priv,
    LD_USP,
    LD_SSP,
    LD_EXCV,
    LD_Vector,
    Gate_PC_2,
    Gate_PSR,
    Gate_SP,
    Gate_Vector,
    PrivMUX,
    SPMUX1, SPMUX0,
    CCMUX,
    VectorMUX,
    /* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetLD_Priv(int *x)         { return(x[LD_Priv]); }
int GetLD_USP(int *x)         { return(x[LD_USP]); }
int GetLD_SSP(int *x)         { return(x[LD_SSP]); }
int GetLD_EXCV(int *x)         { return(x[LD_EXCV]); }
int GetLD_Vector(int *x)         { return(x[LD_Vector]); }
int GetGate_PC_2(int *x)         { return(x[Gate_PC_2]); }
int GetGate_PSR(int *x)         { return(x[Gate_PSR]); }
int GetGate_SP(int *x)         { return(x[Gate_SP]); }
int GetGate_Vector(int *x)         { return(x[Gate_Vector]); }
int GetPrivMUX(int *x)         { return(x[PrivMUX]); }
int GetSPMux(int *x)        { return((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetCCMux(int *x)         { return(x[CCMUX]); }
int GetVectorMUX(int *x)         { return(x[VectorMUX]); }
/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x08000 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int Priv; /* PSR[15] */
int Vector;
int Saved_USP;
int Saved_SSP;
/* MODIFY: You may add system latches that are required by your implementation */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
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

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
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

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 2]);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.Saved_SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.Priv = 1;
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
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.

   Do not modify the rdump and mdump functions.

   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/
#define CMI CURRENT_LATCHES.MICROINSTRUCTION
#define NMI NEXT_LATCHES.MICROINSTRUCTION
#define LowByte(x) ((x) & 0x00FF)
#define HighByte(x) ((x) & 0xFF00)

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

int bitsToDec(int num, int highestBit, int lowestBit, int isSigned) {
    if (!isSigned) {
        return ((num & (power(2, highestBit + 1) - 1 - (power(2, lowestBit) - 1))) >> lowestBit);
    }
    else {
        int ret = ((num & (power(2, highestBit) - 1 - (power(2, lowestBit) - 1))) >> lowestBit);
        return (num & power(2, highestBit)) ? -1 * power(2, highestBit - lowestBit) + ret : ret;
    }
}

#define OPCODE bitsToDec(CURRENT_LATCHES.IR, 15, 12, UNSIGNED)
#define DR GetDRMUX(CMI) == 2 ? 6 : (GetDRMUX(CMI) ? 7 : bitsToDec(CURRENT_LATCHES.IR, 11, 9, UNSIGNED))
#define SR1 GetSR1MUX(CMI) == 2 ? 6 : (bitsToDec(CURRENT_LATCHES.IR, GetSR1MUX(CMI) ? 8 : 11, GetSR1MUX(CMI) ? 6 : 9, UNSIGNED))
#define SR2 bitsToDec(CURRENT_LATCHES.IR, 2, 0, UNSIGNED)
#define A bitsToDec(CURRENT_LATCHES.IR, 5, 5, UNSIGNED)
#define D bitsToDec(CURRENT_LATCHES.IR, 4, 4, UNSIGNED)
#define IMM5 bitsToDec(CURRENT_LATCHES.IR, 4, 0, SIGNED)
#define AMT4 bitsToDec(CURRENT_LATCHES.IR, 3, 0, UNSIGNED)
#define PCO9 bitsToDec(CURRENT_LATCHES.IR, 8, 0, SIGNED)
#define PCO11 bitsToDec(CURRENT_LATCHES.IR, 10, 0, SIGNED)
#define O6 bitsToDec(CURRENT_LATCHES.IR, 5, 0, SIGNED)
#define TRPV8 bitsToDec(CURRENT_LATCHES.IR, 7, 0, UNSIGNED)
#define VECTOR GetVectorMUX(CMI) ? CURRENT_LATCHES.EXCV : CURRENT_LATCHES.INTV

int EXC;

void eval_micro_sequencer() {

    /*
     * Evaluate the address of the next state according to the
     * micro sequencer logic. Latch the next microinstruction.
     */
    if (CYCLE_COUNT == 299) {
        NEXT_LATCHES.INTV = 0x01;
    }
    if (CURRENT_LATCHES.STATE_NUMBER == 47) {
        NEXT_LATCHES.INTV = 0x00;
    }
    int EXC1 = ((CURRENT_LATCHES.MAR & 1) && GetDATA_SIZE(CMI)) || ((CURRENT_LATCHES.MAR <= 0x2FFF) && CURRENT_LATCHES.Priv);
    int EXC0 = ((CURRENT_LATCHES.MAR <= 0x2FFF) && CURRENT_LATCHES.Priv) || (OPCODE == 10 || OPCODE == 11);
    EXC = (EXC1 << 1) + EXC0;
    if (GetIRD(CMI)) {
        NEXT_LATCHES.STATE_NUMBER = OPCODE;
    }
    else if ((GetCOND(CMI) == 6) && EXC1) {
        NEXT_LATCHES.STATE_NUMBER = 63;
    }
    else {
        NEXT_LATCHES.STATE_NUMBER = GetJ(CONTROL_STORE[CURRENT_LATCHES.STATE_NUMBER]) |
                                    (((GetCOND(CMI) == 5) && CURRENT_LATCHES.INTV) << 4) |
                                    (((GetCOND(CMI) == 4) && CURRENT_LATCHES.Priv) << 3) |
                                    (((GetCOND(CMI) == 2) && CURRENT_LATCHES.BEN) << 2) |
                                    (((GetCOND(CMI) == 1) && CURRENT_LATCHES.READY) << 1) |
                                    (((GetCOND(CMI) == 3) && (CURRENT_LATCHES.IR & 0x0800)));
    }
    memcpy(NMI, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int) * CONTROL_STORE_BITS);
}

int MEM_CYCLE = 1;
int MEM_DONE = 0;

void cycle_memory() {
    /*
     * This function emulates memory and the WE logic.
     * Keep track of which cycle of MEMEN we are dealing with.
     * If fourth, we need to latch Ready bit at the end of
     * cycle to prepare microsequencer for the fifth cycle.
     */
    if (GetMIO_EN(CMI)) {
        if (MEM_CYCLE == MEM_CYCLES - 1) {
            NEXT_LATCHES.READY = 1;
            MEM_CYCLE++;
        }
        else if (MEM_CYCLE == MEM_CYCLES) {
            MEM_DONE = 1;
            MEM_CYCLE = 1;
        }
        else {
            NEXT_LATCHES.READY = 0;
            MEM_CYCLE++;
        }
    }
    else {
        NEXT_LATCHES.READY = 0;
        MEM_CYCLE = 1;
    }
}

int MARMUX_BUS, PC_BUS, ALU_BUS, SHF_BUS, MDR_BUS, PC_2_BUS, PSR_BUS, SP_BUS, Vector_BUS; // Values to be driven on bus

int eval_addr1() {
    switch (GetADDR1MUX(CMI)) {
        case 0:
            return CURRENT_LATCHES.PC;
        case 1:
            return CURRENT_LATCHES.REGS[SR1];
    }
}

int eval_addr2() {
    switch (GetADDR2MUX(CMI)) {
        case 0:
            return 0;
        case 1:
            return O6 << GetLSHF1(CMI);
        case 2:
            return PCO9 << GetLSHF1(CMI);
        case 3:
            return PCO11 << GetLSHF1(CMI);
    }
}

void eval_bus_drivers() {

    /*
     * Datapath routine emulating operations before driving the bus.
     * Evaluate the input of tristate drivers
     *       Gate_MARMUX,
     *		 Gate_PC,
     *		 Gate_ALU,
     *		 Gate_SHF,
     *		 Gate_MDR.
     */

    if (GetMARMUX(CMI)) {
        MARMUX_BUS = Low16bits(eval_addr1() + eval_addr2());
    }
    else {
        MARMUX_BUS = Low16bits(TRPV8 << 1);
    }
    PC_BUS = CURRENT_LATCHES.PC;
    int OP2 = A ? IMM5 : CURRENT_LATCHES.REGS[SR2];
    switch (GetALUK(CMI)) {
        case 0:
            ALU_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1] + OP2);
            break;
        case 1:
            ALU_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1]  & OP2);
            break;
        case 2:
            ALU_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1]  ^ OP2);
            break;
        case 3:
            ALU_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1]);
    }
    if (A) { // RSHFA
        if (bitsToDec(CURRENT_LATCHES.REGS[SR1], 15, 15, UNSIGNED)) {
            int TEMP = CURRENT_LATCHES.REGS[SR1];
            for (int i = AMT4; i > 0; i--) {
                TEMP = (TEMP >> 1) + 0x8000;
            }
            SHF_BUS = Low16bits(TEMP);
        }
        else {
            SHF_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1]  >> AMT4);
        }
    }
    else if (D) { // RSHFL
        SHF_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1] >> AMT4);
    }
    else { // LSHF
        SHF_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1] << AMT4);
    }
    if (GetDATA_SIZE(CMI)) {
        MDR_BUS = CURRENT_LATCHES.MDR;
    }
    else {
        if (CURRENT_LATCHES.MAR & 1) {
            MDR_BUS = Low16bits(bitsToDec(CURRENT_LATCHES.MDR, 15, 8, SIGNED));
        }
        else {
            MDR_BUS = Low16bits(bitsToDec(CURRENT_LATCHES.MDR, 7, 0, SIGNED));
        }
    }
    PC_2_BUS = Low16bits(CURRENT_LATCHES.PC - 2);
    PSR_BUS = Low16bits((CURRENT_LATCHES.Priv << 15) + (CURRENT_LATCHES.N << 2) + (CURRENT_LATCHES.Z << 1) + CURRENT_LATCHES.P);
    switch (GetSPMux(CMI)) {
        case 0:
            SP_BUS = CURRENT_LATCHES.Saved_USP;
            break;
        case 1:
            SP_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1] + 2);
            break;
        case 2:
            SP_BUS = Low16bits(CURRENT_LATCHES.REGS[SR1] - 2);
            break;
        case 3:
            SP_BUS = CURRENT_LATCHES.Saved_SSP;
    }
    Vector_BUS = Low16bits(0x0200 + (CURRENT_LATCHES.Vector << 1));
}


void drive_bus() {

    /*
     * Datapath routine for driving the bus from one of the 5 possible
     * tristate drivers.
     */

    if (GetGATE_MARMUX(CMI)) {
        BUS = MARMUX_BUS;
    }
    else if (GetGATE_PC(CMI)) {
        BUS = PC_BUS;
    }
    else if (GetGATE_ALU(CMI)) {
        BUS = ALU_BUS;
    }
    else if (GetGATE_SHF(CMI)) {
        BUS = SHF_BUS;
    }
    else if (GetGATE_MDR(CMI)) {
        BUS = MDR_BUS;
    }
    else if (GetGate_PC_2(CMI)) {
        BUS = PC_2_BUS;
    }
    else if (GetGate_PSR(CMI)) {
        BUS = PSR_BUS;
    }
    else if (GetGate_SP(CMI)) {
        BUS = SP_BUS;
    }
    else if (GetGate_Vector(CMI)) {
        BUS = Vector_BUS;
    }
    else {
        BUS = 0;
    }
}


void latch_datapath_values() {

    /*
     * Datapath routine for computing all functions that need to latch
     * values in the data path at the end of this cycle.  Some values
     * require sourcing the bus; therefore, this routine has to come
     * after drive_bus.
     */
    if (GetLD_MAR(CMI)) {
        NEXT_LATCHES.MAR = BUS;
    }
    if (GetLD_MDR(CMI) && GetMIO_EN(CMI)) {
        if (MEM_DONE) {
            NEXT_LATCHES.MDR = Low16bits(
                    (MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) + MEMORY[CURRENT_LATCHES.MAR >> 1][0]);
        }
        MEM_DONE = 0;
    }
    else if (GetLD_MDR(CMI)) {
        NEXT_LATCHES.MDR = (CURRENT_LATCHES.MAR & 1) ? (bitsToDec(BUS, 7, 0, UNSIGNED) << 8) + bitsToDec(BUS, 7, 0, UNSIGNED) : BUS;
    }
    if (GetR_W(CMI) && GetMIO_EN(CMI)) {
        if (MEM_DONE) {
            if (GetDATA_SIZE(CMI)) { // STW - DATA_SIZE = WORD, WE0 = 1, WE1 = 1
                MEMORY[CURRENT_LATCHES.MAR >> 1][0] = LowByte(CURRENT_LATCHES.MDR);
                MEMORY[CURRENT_LATCHES.MAR >> 1][1] = HighByte(CURRENT_LATCHES.MDR) >> 8;
            } else { // STB - DATA_SIZE = WORD, WE0 = !MAR[0], WE1 = MAR[0]
                MEMORY[CURRENT_LATCHES.MAR >> 1][CURRENT_LATCHES.MAR & 1] = (CURRENT_LATCHES.MAR & 1) ?
                                                                            HighByte(CURRENT_LATCHES.MDR) >> 8 : LowByte(CURRENT_LATCHES.MDR);
            }
            MEM_DONE = 0;
        }
    }
    if (GetLD_BEN(CMI)) {
        NEXT_LATCHES.BEN = CURRENT_LATCHES.N & (bitsToDec(CURRENT_LATCHES.IR, 11, 11, UNSIGNED)) ||
                           CURRENT_LATCHES.Z & (bitsToDec(CURRENT_LATCHES.IR, 10, 10, UNSIGNED)) ||
                           CURRENT_LATCHES.P & (bitsToDec(CURRENT_LATCHES.IR, 9, 9, UNSIGNED));
    }
    if (GetLD_IR(CMI)) {
        NEXT_LATCHES.IR = BUS;
    }
    if (GetLD_REG(CMI)) {
        NEXT_LATCHES.REGS[DR] = BUS;
    }
    if (GetLD_CC(CMI)) {
        int signedBus = bitsToDec(BUS, 15, 0, SIGNED);
        if (signedBus < 0) {
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else if (signedBus == 0) {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if (signedBus > 0) {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
    }
    if (GetLD_PC(CMI)) {
        switch (GetPCMUX(CMI)) {
            case 0:
                NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
                break;
            case 1:
                NEXT_LATCHES.PC = BUS;
                break;
            case 2:
                NEXT_LATCHES.PC = Low16bits(eval_addr1() + eval_addr2());
        }
    }
    if (GetLD_Priv(CMI)) {
        if (GetPrivMUX(CMI)) {
            NEXT_LATCHES.Priv = 0;
        }
        else {
            NEXT_LATCHES.Priv = BUS && 0x8000;
        }
    }
    if (GetLD_USP(CMI)) {
        NEXT_LATCHES.Saved_USP = CURRENT_LATCHES.REGS[SR1];
    }
    if (GetLD_SSP(CMI)) {
        NEXT_LATCHES.Saved_SSP = CURRENT_LATCHES.REGS[SR1];
    }
    if (GetLD_EXCV(CMI)) {
        switch (EXC) {
            case 1:
                NEXT_LATCHES.EXCV = 0x04;
                break;
            case 2:
                NEXT_LATCHES.EXCV = 0x03;
                break;
            case 3:
                NEXT_LATCHES.EXCV = 0x02;
        }
    }
    if (GetLD_Vector(CMI)) {
        NEXT_LATCHES.Vector = VECTOR;
    }
}
