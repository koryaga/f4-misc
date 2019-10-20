#ifndef F4CPU_H
#define F4CPU_H
#include <stdbool.h>
// http://www.dakeng.com/misc.html

// word size. For now 16bit
typedef unsigned short int w_size;

// The state of cpu. Should be filled initially
struct STATE{            
  w_size a;             // A register. Accumulator
  w_size pc;            // Program Counter 
  bool overflow;        // overflow flag
  // Non F4 CPU specific data
  unsigned int size;    // size of code in words (w_size - 16 bit currently)
  struct INSTR *code;   // pointer to code array 
};

// possible CPU instructions
enum INSTRUCTIONS{
  ADDi=1, // imm+(A) --> A 
  ADDm,   // (addr)+(A) --> A
  ADDpc,  // PC+(A) --> A
  BVS,    // (addr) --> PC if <v>=1
  LDAi,   // imm --> A 
  LDAm,   // (addr) --> A
  LDApc,  // PC --> A
  STAm,   // A --> (addr)
  STApc   // A --> PC
};

#define NO_PARAM 0 // to pass no parameters

// format of one instruction
struct INSTR{
  w_size cmd;   //INSTRUCTIONS
  w_size param; // 16 bit , 0 in case no param
};

/* Run one cpu iteration 
   Input:
      STATE - CPU current state 
   Return:
      0 - SUCCESS
      1 - ERROR
 */
int RunIteration(struct STATE *);

/* Reset to zero/default values */
void Reset(struct STATE *);
#endif

