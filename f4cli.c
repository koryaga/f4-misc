#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <f4.h>
#include <cli.h>

struct STATE st; //F4 CPU state
int end=0; //instruction end in the memory. Index to set new instruction during editing

bool intMode=true; //how to process input non-interactive (pipe) or interactive (stdin, using commands)

int print_state();
int print_code();
int get_instr(char *);
int set_instr(char *param[]);
int edit_state(char *param[]);
int set_instr_index(char *param[]);
int run_iteration_cmd();

char cpu_instr[][MAX_CMDPARAM_SIZE+1]={ //appropriate instraction names for INSTRUCTIONS. Must be in the same positions as their instructions
    "ADDi", "ADDm", "ADDpc", "BVS", "LDAi", "LDAm", "LDApc", "STAm", "STApc" };


struct cmd cmd_names[]={
    {"e",edit_state,3}, //edit state
    {"ps",print_state,0},
    {"pc",print_code,0},
    {"i",set_instr_index,1},
    //{"\n",run_iteration_cmd,0},   //default command ; <Enter> 
    {"r",run_iteration_cmd,0}, 
    {cpu_instr[0],set_instr,1},    
    {cpu_instr[1],set_instr,1},    
    {cpu_instr[2],set_instr,0},    
    {cpu_instr[3],set_instr,1},    
    {cpu_instr[4],set_instr,1},    
    {cpu_instr[5],set_instr,1},    
    {cpu_instr[6],set_instr,0},    
    {cpu_instr[7],set_instr,1},    
    {cpu_instr[8],set_instr,0},    
    {NULL,NULL,0}
  };

//edit state; change instr edit index; print state; print code(index); run iteration; set instr
int set_instr_index(char *param[]){
  end = atoi(param[1]);
}

int run_iteration_cmd(){
  run_iteration(&st);
}

int edit_state(char *param[]){ // param[1]=a; pc; overflow; siza
  st.a = atoi(param[1]);
  st.pc = atoi(param[2]);
  st.overflow = atoi(param[3]);
  //TODO st.size = atoi(param[4]);
  //TODO need to realloc real code size if needed
}

int set_instr(char *param[]){
  struct INSTR is;
  int instr=get_instr(param[0]);
  is.cmd = instr;
  is.param = ( (param[1]!=NULL)?atoi(param[1]):0 );
  memcpy((st.code+end/2),&is,sizeof(is));
  end+=2;
  if(intMode==false)  //in pipe mode run instruction immediatly, in non-iner mode just insert it in memory
    run_iteration(&st);
}

// argv[1] - mem size in 16bit words
int main(int argc, char *argv[]){
  w_size param;
  struct INSTR is;
  char ps1[100];
  
  // set mode interactive or not
  struct stat stat;
  fstat(STDIN_FILENO,&stat);
  intMode=(stat.st_mode & S_IFIFO)?false:true;

  //init cpu
  int size=atoi((argv[1]==NULL)?"100":argv[1]); //100 w_size if not passed
  st.a=0;
  st.pc=0;
  st.size=size;
  st.overflow=false;
  st.code=(struct INSTR *) malloc(size*sizeof(w_size)); //allocate memory in w_size (16bit) chunks
  while(1){
    sprintf(ps1,"\nF4 state(A=%i PC=%i Over=%i) Enter command[%i]:",st.a,st.pc,st.overflow,end);
    int ret=cli_handler(ps1,cmd_names);
    if(ret==-1)
      return 0;
    else if(ret==1)
      printf("Wrong command or parameter(s)");
      
  }
}


int get_instr(char *name){

 for(int i=0;i<sizeof(cpu_instr)/(MAX_CMDPARAM_SIZE+1);i++){
    if(!strncmp(cpu_instr[i],name,MAX_CMDPARAM_SIZE)) 
      return ++i; //instruction index starts from 1
  }
  return 0;// command not found
}


int print_state(){
  printf("A=%i PC=%i Over=%i Size=%i\n",st.a,st.pc,st.overflow,st.size);
}

int print_code(){
  printf("\n");
  for(int i=0;i<st.size/2;i++)
    printf("%i [%s] [%i]\n",i+i,cpu_instr[st.code[i].cmd-1],st.code[i].param);
}
