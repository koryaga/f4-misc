#include <stdio.h>
#include <f4.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define INSTR_L 5  //length of instruction command name

static void print_state(struct STATE *);
static void print_code(struct STATE *);
static int  get_instr(char *);
static parse_cmd(char *s);

struct {
  char 
}cmd;

static char* cmds[]={"editstate","instrindex","printstate","printcode","run","setinstr"};

//edit state; change instr edit index; print state; print code(index); run iteration; set instr

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
  }
  print_code(&st);
}

static  char *cpu_instr[]={ //appropriate instraction names for INSTRUCTIONS
    "ADDi", "ADDm", "ADDpc", "BVS", "LDAi", "LDAm", "LDApc", "STAm", "STApc" };


static int get_instr(char *name){
 for(int i=0;i<sizeof(cpu_instr)/sizeof(char*);i++){
    if(!strncmp(cpu_instr[i],name,INSTR_L)) //TODO better to use something more regexp friendly
      return ++i; //instruction index starts from 1
  }
  return 0;// command not found
}


void print_state(struct STATE *st){
  printf("A=%i PC=%i Over=%i Size=%i\n",st->a,st->pc,st->overflow,st->size);
}

void print_code(struct STATE *st){
  printf("\n");
  for(int i=0;i<st->size/2;i++)
    printf("%i [%s] [%i]\n",i+i,cpu_cmd[st->code[i].cmd-1],st->code[i].param);
}
