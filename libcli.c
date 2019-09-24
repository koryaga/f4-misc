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
bool cli_handler(char *promt, struct cmd cmd_name[]){
  bool ret=true;
  
  char *argv_c[MAX_PARAM_COUNT+1]={NULL}; // +1 for command name; argv_c[0] - command name; argv_c[1...] params
  regex_t re;
  regmatch_t matches[MAX_PARAM_COUNT*2+2];  //  2=command match + end garbage match 
  char str[MAX_CMDPARAM_SIZE+(MAX_PARAM_COUNT*(MAX_CMDPARAM_SIZE+1))+1];
  
  char regex_cmdname[]="^[[:space:]]*([a-zA-Z]+|\n{1})"; // command name regex. Begining of regex ^  . \n - default command
  //char regex_cmdname[]="^[[:space:]]*([a-zA-Z]+)"; // command name regex. Begining of regex ^  . 
  char regex_param[]="([[:space:]]+([[:alnum:]]+))?"; // number or symb parameter regex :alnum:
  char regex_spec_symb[]="[[:space:]]*$"; // space/tab/newline regex. End of regex $
  char *regex_cmd=(char*)malloc(sizeof(regex_cmdname)+(MAX_PARAM_COUNT*sizeof(regex_param)+sizeof(regex_spec_symb))+1); // +1(\0)

  //making regex string
  regex_cmd[0]='\0';
  strcat(regex_cmd,regex_cmdname);
  for(int i=1;i<=MAX_PARAM_COUNT;i++)
    strcat(regex_cmd,regex_param);
  strcat(regex_cmd,regex_spec_symb);

  int ret_code = regcomp(&re,(const char*)regex_cmd, REG_EXTENDED);
  
  printf("%s",promt);
  fgets(str,sizeof(str),stdin);
  
  //apply regexp
  ret_code = regexec(&re, str,sizeof(matches)/sizeof(matches[0]),(regmatch_t*)&matches,0);
  if (ret_code!=0){
    free(regex_cmd);
    regfree(&re);
    return false;//not match
  }

  //copy command and each parsed parameter usinf malloc. Need a free! 
  int match_cnt=0;
  for(int i=0;i<MAX_PARAM_COUNT+1;i++){
   if(matches[i*2+1].rm_so != -1){  // 0-is a whole word; 1-cmdname;2-spaces;3-param1;4-spaces;5-param2 ....
      argv_c[i]=strndup(str+matches[i*2+1].rm_so,matches[i*2+1].rm_eo - matches[i*2+1].rm_so);
      argv_c[i][matches[i*2+1].rm_eo - matches[i*2+1].rm_so]='\0';
      match_cnt++;
    }
  }

  // find related command and run it with params
  for(int i=0;cmd_name[i].name!=NULL;i++){
    if(strncmp(argv_c[0],cmd_name[i].name,MAX_CMDPARAM_SIZE) == 0){
      if(cmd_name[i].argc==match_cnt-1) //if params match
        (cmd_name[i].ff)(argv_c);
      else
        ret=false; //passed params not matched for command
      
      break;
    }
  }
  
  for(int i=0;i<match_cnt;i++)
    free(argv_c[i]);
  
  free(regex_cmd);
  regfree(&re);
  
  return ret;
}

