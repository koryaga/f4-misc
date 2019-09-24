#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

#define MAX_CMDPARAM_SIZE 5    
#define MAX_PARAM_COUNT 2


struct cmd{
    char *name;
    int (*ff)(char *ss[]);
    int argc;
};

// 1 - promt string; 2 - filled cmd array
bool cli_handler(char *promt, struct cmd cmd_name[]);


#endif
