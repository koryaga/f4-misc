#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

#define MAX_CMDPARAM_SIZE 5    
#define MAX_PARAM_COUNT 4


struct cmd{
    char *name;
    int (*ff)(char *ss[]);
    int argc;
};

/* Blocking loop to handle single command from cmd_name array
   Input:
      promt string
      filled cmd array
   Returns:
      0  - ok
      1  - wrong command 
      -1 - EOF 
*/
int HandleSingleCmd(char *promt, struct cmd cmd_name[]);


#endif
