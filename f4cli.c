#include <stdio.h>
#include <f4.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define INSTR_L 5  //length of instruction command name

static int get_instr(char *);

int main(int argc, char *argv[]){
  char instr[INSTR_L+1];
  w_size param;
  struct STATE st;
  struct INSTR is;
  int end=0; //instruction end in the memory
  //init cpu
  st.a=0;
  st.pc=0;
  st.size=atoi(argv[1]);
  st.overflow=false;
  st.code=(struct INSTR *) malloc(atoi(argv[1])*sizeof(w_size)); //allocate memory in w_size (16bit) chunks
  
  for(;;){
    print_state(&st);
    printf("\nf4[%i]>",end+end);
    int ret = scanf("%s %hi",instr,&is.param);
    if(!(is.cmd=get_instr(instr))){
      printf("Wrong command\n");
      continue;
    }
    if (ret==-1){
       if(!run_iteration(&st))
          break;
    }
    else{
      memcpy((st.code+end),&is,sizeof(is));
      end++;
      if(end >= st.size/(sizeof(struct INSTR)/sizeof(w_size))) end=0; 
    }  
    print_code(&st);
  
  }
}

static int get_instr(char *name){
  char *cpu_cmd[]={ //appropriate command names for INSTRUCTIONS
    "ADDi", "ADDm", "ADDpc", "BVS", "LDAi", "LDAm", "LDApc", "STAm", "STApc" };
  for(int i=0;i<sizeof(cpu_cmd)/sizeof(char*);i++){
    if(!strncmp(cpu_cmd[i],name,INSTR_L)) //TODO better to use something more regexp friendly
      return ++i; //instruction index starts from 1
  }
  return 0;// command not found
}