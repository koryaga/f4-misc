#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <regex.h>
#include <cli.h>

#ifdef TEST
int func3(char *i[]){
  printf("%s %i %s",i[0],atoi(i[1]),i[2]);
  return 0;
}


int func(char *i[]){
  printf("%s",i[0]);
  return 0;
}

int func2(){
  printf("func2");
  return 0;
}

int main(int argc, char *argv[]){

  struct cmd cmd_names[]={
    {"est",func2,0},
    {"ind",func,1},
    {"prn",func3,2}, 
    {"exit",func3,1},
    {"\n",func2,0},   //default command ; <Enter>
    {NULL,NULL,0}
  };

  while(1)
    cli_handler("\nEnter command: ",cmd_names);

}

#endif

//parses single command string according to cmd_names
int HandleSingleCmd(char *promt, struct cmd cmd_name[],FILE *inputStream){
  bool ret=0;
  
  char *argv_c[MAX_PARAM_COUNT+1+1]={NULL}; // +1 for command name; +1 for pc; argv_c[0] - command name; argv_c[1...] params; pc
  regex_t re;
  regmatch_t matches[MAX_PARAM_COUNT*2+2];  //  2=command match + end garbage match 
  char str[5+MAX_CMDPARAM_SIZE+(MAX_PARAM_COUNT*(MAX_CMDPARAM_SIZE+1))+1];
  //                 line number                     command                     parameters                          comments  
  char regex_cmd[]="^([[:xdigit:]]{0,5}[[:blank:]]+)?([a-zA-Z]+)([[:blank:]]{1,3}([A-Za-z0-9.]+)){0,3}[[:blank:]]{0,}[#[[:blank:]]{0,}[[:alnum:]]{0,}]?.*$";
  
  int ret_code = regcomp(&re,(const char*)regex_cmd, REG_EXTENDED);
  
  printf("%s",promt);
  if(fgets(str,sizeof(str),inputStream)==NULL)
    return -1; //EOF
  
  //apply regexp
  ret_code = regexec(&re, str,sizeof(matches)/sizeof(matches[0]),(regmatch_t*)&matches,0);
  if (ret_code!=0){
    regfree(&re);
    return 1;//not match
  }
 

  //copy command and each parsed parameter using malloc. Need a free! 
  int match_cnt=0;
  for(int i=0;i<MAX_PARAM_COUNT+1;i++){
    
    // 0-is a whole word; 1-pc; 2-cmdname; 3-spaces+param; 4-param1; 5-spaces+param; 6-param2 ....
    if(matches[i*2+2].rm_so == -1)
      break; 

    argv_c[i]=strndup(str+matches[i*2+2].rm_so,matches[i*2+2].rm_eo - matches[i*2+2].rm_so);
    argv_c[i][matches[i*2+2].rm_eo - matches[i*2+2].rm_so]='\0';
    match_cnt++;
  }

  //copy pc number to the last param
  argv_c[match_cnt]=strndup(str+matches[1].rm_so,matches[1].rm_eo - matches[1].rm_so);
  argv_c[match_cnt][matches[1].rm_eo - matches[1].rm_so]='\0';

//for(int i=0;i<=match_cnt;i++)
//printf("---%s\n",argv_c[i]);

  // find related command and run it with params
  for(int i=0;cmd_name[i].name!=NULL;i++){
    if(strncmp(argv_c[0],cmd_name[i].name,MAX_CMDPARAM_SIZE) == 0){
      if(cmd_name[i].argc==match_cnt-1 || cmd_name[i].mandatory==false){ //if params match or param is not mandatory
        (cmd_name[i].ff)(argv_c);
        ret=0;
      }
      else
        if(cmd_name[i].mandatory==true)
          ret=1; //passed params not matched for command
      break;
    
    }else
      ret=1;    
  }
  
  for(int i=0;i<match_cnt;i++)
    free(argv_c[i]);
  
//  free(regex_cmd);
  regfree(&re);
  
  return ret;
}

