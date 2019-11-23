#include <stdio.h>
#include <f4.h>
#include <limits.h>
#include <errno.h>

#ifdef TEST
//simple test with fixed code
int main(){
  // 7777+7->A=7784
  struct INSTR ss[]={
    {LDAi,7777},
    {ADDi ,7},
    {STAm, 1},  // 7784->*1
    {ADDm, 1},  // 7784+7784=15568
    {ADDpc,NO_PARAM}, //8+15568=15576
    {ADDi ,49962},    // 15576+49962=2(65535) v=true
    {NO_PARAM ,NO_PARAM},
    {BVS, 18},
    {NO_PARAM ,NO_PARAM},
    {STApc,NO_PARAM}, // goto PC=2 v=true
  };
 
  struct STATE  st;
  Reset(&st);
  
  st.size=20;
  st.code=ss;
  
  for(int i=0;i<(st.size/2);i++){ 
    if(RunIteration(&st))
      { errno=-1; perror("Error"); }
    
  }
  //A=9 PC=4 Over=0
  if(st.a!=9 || st.pc!=4 || st.overflow!=0 ) { //test failed
    errno = -1; //Unkown error
    perror("Test failed\n");
    return 1;
  }else{
    printf("Test passed\n");
    return 0;
  }
}
#endif

int RunIteration(struct STATE *state ){
  #define A state->a  
  #define PC state->pc
  #define CMD state->code[PC/(sizeof(struct INSTR)/sizeof(w_size))].cmd
  #define PARAM state->code[PC/(sizeof(struct INSTR)/sizeof(w_size))].param
  
  if( (state == NULL || state->code == NULL ))
    return 1;
  
  if( (CMD == ADDm || CMD == BVS || CMD == LDAm || CMD == STAm) && (PARAM > state->size) ||
      ( PC >= state->size ))
    return 1; //  next instruction or mem param out of programm memory/code
    
  

  switch(CMD){ 
    case ADDi:
      if (A+PARAM > USHRT_MAX )  //TODO USHRT_MAX should depends on w_size
        state->overflow=true;
      else
        state->overflow=false;
      A=A+PARAM;
      PC+=2;
      break;
    
    case ADDm:
      if (A+*((w_size*)(state->code) + PARAM) > USHRT_MAX )
        state->overflow=true;
      else
        state->overflow=false;
      A=A+*((w_size*)(state->code) + PARAM);
      PC+=2;
      break;
    
    case ADDpc:
      if ( A+PC > USHRT_MAX )
        state->overflow=true;
      else
        state->overflow=false;
      A=A+PC;
      PC+=2;
      break;
    
    case BVS:
      if (state->overflow)
        PC=PARAM;
      else
        PC+=2;
      break;
    
    case LDAi:
      A=PARAM;
      PC+=2;
      break;
 
    case LDAm:
      A=*((w_size*)(state->code) + PARAM);
      PC+=2;
      break;
    
    case LDApc:
      A=PC;
      PC+=2;
      break;
    
    case STAm:
      *((w_size*)(state->code) + PARAM)=A;
      PC+=2;
      break;
    
    case STApc:
      PC=A;
      break;
    
    default: //let's assume unknown instruction, however go next
      PC+=2;
      break;
  }

  return 0; //SUCCESS
}

void Reset(struct STATE *st){
  st->a=0;
  st->pc=0;
  st->overflow=false;
}
