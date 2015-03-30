#include<stdlib.h>
#include<string.h>
#include"cmd.h"

#define T cmd_t

struct T{
    char cmd[32];
    int (*fun)(char* arg, void *cl);
};

T       
cmd_new(char *cmd, int (*fun)(char* arg, void *cl))
{
    T ret_val;


    ret_val = malloc(sizeof(*ret_val));

    ret_val->fun = fun;
    strncpy(ret_val->cmd, cmd, 32);
    return ret_val;
}


void    
cmd_free(T *cmd)
{
    free(*cmd);
}

int     
cmd_cmp(T cmd, char *cmd_str)
{
    return stricmp(cmd_str, cmd->cmd);
}

int     
cmd_run(T cmd, char *arg, void *cl)
{
    return cmd->fun(arg, cl);
}
