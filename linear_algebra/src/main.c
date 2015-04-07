#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"matrix.h"
#include"handler.h"
#include"table.h"

#define PROMPT      "mt>"

int
main(int argc, char *argv[])
{
    int         ret;
    char        cmd[CMD_LEN];
    table_t     obj_table;
    handler_t   h;


    obj_table = table_new(0, NULL, NULL);

    h = handler_new();

    while(1){
        printf(PROMPT);
        fgets(cmd, CMD_LEN, stdin);
        ret = handler_run(h, cmd, obj_table);
        if(ERR_QUIT == ret){
            break;
        }else if(ERR_BAD_CMD == ret){
            handler_print(h);
        }
    }

    handler_free(&h);
}

