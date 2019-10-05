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

// 1 - promt string; 2 - filled cmd array
// 0 - ok; 1 - wrong command ; -1 - EOF
int cli_handler(char *promt, struct cmd cmd_name[]);


#endif
