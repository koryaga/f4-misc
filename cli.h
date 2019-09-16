#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

#define MAX_PARAM_SIZE 10
#define MAX_CMD_SIZE 5
#define MAX_PARAM_COUNT 2


struct cmd{
    char *name;
    int (*ff)(char *ss[]);
    int argc;
};

bool cli_handler(struct cmd cmd_name[]);


#endif
