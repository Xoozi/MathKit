#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"matrix.h"
#include"handler.h"

#define PROMPT      "mt>"

static  void    _print_help(char *argv[]);
static  void    _display_matrix(matrix_t m);


int
main(int argc, char *argv[])
{
    char        cmd[CMD_LEN];
    int         row, col, ret;
    matrix_t    m;
    handler_t   h;

    if(3 != argc){
        _print_help(argv);
        return 1;
    }

    row = atoi(argv[1]);
    col = atoi(argv[2]);

    m = matrix_new(row, col);
    h = handler_new();

    while(1){
        printf(PROMPT);
        fgets(cmd, CMD_LEN, stdin);
        ret = handler_run(h, cmd, m);
        if(ERR_QUIT == ret){
            break;
        }else if(ERR_BAD_CMD == ret){
            handler_print(h);
        }
    }

    handler_free(&h);
    matrix_free(&m);
}

static
void
_print_help(char *argv[])
{
    printf("Usage of %s: %s row col\n"
            "row and col is num\n",
            argv[0], argv[0]);
}


