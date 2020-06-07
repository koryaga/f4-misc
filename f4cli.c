#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <f4.h>
#include <cli.h>

static struct STATE st; //F4 CPU state
static int end=0; //instruction end in the memory. Index to set new instruction during editing

static bool intMode=true; //how to process input non-interactive (pipe) or interactive (stdin, using commands)

int print_state();
int disassemble();
int get_instr(char *);
int set_instr(char *param[]);
int edit_state(char *param[]);
int set_instr_index(char *param[]);
int run_iteration_cmd(char *param[]);
int loadCodeFromFile(char *param[]);

char cpu_instr[][MAX_CMDPARAM_SIZE+1]={ //appropriate instraction names for INSTRUCTIONS. Must be in the same positions as their instructions
    "ADDi", "ADDm", "ADDpc", "BVS", "LDAi", "LDAm", "LDApc", "STAm", "STApc" };


struct cmd cmd_names[]={
    {"e",edit_state,3,true},           // edit F4 CPU state A= PC= Over=
    {"ps",print_state,0,true},         // print F4state
    {"ds",disassemble,2,false},         // print code . no_params - all; {1} - start index  {2} - number of bytes
    {"i",set_instr_index,1,true},      // edit instruction index where to put next CPU instruction. Interactive mode
 //   {"\n",set_instr,0,true},           // for non interactive mode, to set 0 0 values 
    {"r",run_iteration_cmd,1,false},    // run  CPU instruction 
    {"load",loadCodeFromFile,1,true},    // run single CPU instruction 
    {cpu_instr[0],set_instr,1,true},    
    {cpu_instr[1],set_instr,1,true},    
    {cpu_instr[2],set_instr,0,true},    
    {cpu_instr[3],set_instr,1,true},    
    {cpu_instr[4],set_instr,1,true},    
    {cpu_instr[5],set_instr,1,true},    
    {cpu_instr[6],set_instr,0,true},    
    {cpu_instr[7],set_instr,1,true},    
    {cpu_instr[8],set_instr,0,true},    
    {NULL,NULL,0}
  };

int set_instr_index(char *param[]){
  end = atoi(param[1]);
}

int run_iteration_cmd(char *param[]){
  int numberOfInstr = ( (param[1]!=NULL)?atoi(param[1]):-1 ); //run defined number of instr or all(-1) till the end 

  while(!RunIteration(&st)){
   if(--numberOfInstr==0)
    return 1;
  }
  return 0;
}

int edit_state(char *param[]){ // param[1]=a; pc; overflow; 
  st.a = atoi(param[1]);
  st.pc = atoi(param[2]);
  st.overflow = atoi(param[3]);
  //TODO st.size = atoi(param[4]);
  //TODO need to realloc real code size if needed
}

// set instruction into program memory 
int set_instr(char *param[]){
  struct INSTR is;
  int instr=((param[0][0]=='\n')?0:get_instr(param[0]));
  is.cmd = instr;
  int pc=-1;

  for(int i=0;cmd_names[i].name!=NULL;i++){
    if(strncmp(param[0],cmd_names[i].name,MAX_CMDPARAM_SIZE) == 0){
      if (cmd_names[i].argc==1){ //only CPU commands has 0 or 1 parameters
        is.param = ( (param[1]!=NULL)?atoi(param[1]):0 );
        pc = ( (strnlen(param[2],MAX_CMDPARAM_SIZE)!=0)?atoi(param[2]):end );
      }
      else{
        is.param = 0;
        pc = ( (strnlen(param[1],MAX_CMDPARAM_SIZE)!=0)?atoi(param[1]):end );
      }
    }
  }

  if(pc != -1)
    end=pc;

  if(end<st.size){
    memcpy((st.code+end/2),&is,sizeof(is));
    end+=2;
  }
}

// argv[1] - mem size in 16bit words
int main(int argc, char *argv[]){
  w_size param;
  struct INSTR is;
  char ps1[100];
  
  // set mode interactive or not depending on stdin descriptor
  struct stat stat;
  fstat(STDIN_FILENO,&stat);
  intMode=(stat.st_mode & S_IFIFO)?false:true;

  //init cpu
  int size=atoi((argv[1]==NULL)?"50":argv[1]); //100 w_size if not passed
  st.a=0;
  st.pc=0;
  st.size=size;
  st.overflow=false;
  st.code=(struct INSTR *) malloc(size*sizeof(w_size)); //allocate memory in w_size (16bit) chunks
  while(1){
    sprintf(ps1,"\nF4 state(A=%i PC=%i Over=%i) Enter command[%i]:",st.a,st.pc,st.overflow,end);
    int ret=HandleSingleCmd((intMode)?ps1:"",cmd_names,stdin);

    if(ret==-1 && intMode==false){  //in non interactive mode run commands once them all read
      char *tt[]={"all",NULL,NULL};
      disassemble(tt);
      while(!RunIteration(&st));       
      
      print_state();
      return 0;
    }
    else if(ret==1 && intMode==true)
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


int loadCodeFromFile(char *param[]){
  FILE *FILE_IN = fopen(param[1],"r");
  int ret;
  
  end=0;
  Reset(&st);

  while((ret=HandleSingleCmd("",cmd_names,FILE_IN))!=-1){}

  fclose(FILE_IN);
}


int disassemble(char *param[]){
  int startIndex = (param[1]==NULL?0:atoi(param[1]));
  int size;
  
  if(param[1]!=NULL) 
    size = (param[2]==NULL?sizeof(w_size):atoi(param[2])); //print 2 bytes by default
  else
    size = (param[2]==NULL?st.size:atoi(param[2]));  //print all(st.size) if no params at all
  
  printf("\n");
  for(int i=startIndex/2;i<(startIndex+size)/2;i++)
    printf("%i [%s] [%i]\n",i+i,cpu_instr[st.code[i].cmd-1],st.code[i].param);
}
