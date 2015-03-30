#include<stdio.h>
#include<stdlib.h>
#include"matrix.h"

#define CMD_LEN         1024
#define CMD_PROMPT      "mt>"
#define CMD_ENTER       ""
#define CMD_QUIT        "q"
#define CMD_EXCHANGE    "ex"

static  void    _print_help(char *argv[]);
static  int     _cmd_handle(char *cmd, matrix_t m);
static  void    _display_matrix(matrix_t m);




int
main(int argc, char *argv[])
{
    char cmd[CMD_LEN];
    int row, col;
    matrix_t m;

    if(3 != argc){
        _print_help(argv);
        return 1;
    }

    row = atoi(argv[1]);
    col = atoi(argv[2]);

    m = matrix_new(row, col);

    while(1){
        printf(CMD_PROMPT);
        fgets(cmd, CMD_LEN, stdin);
        if(!_cmd_handle(cmd))
            break;
    }

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


static
int
_cmd_handle(char *cmd, matrix_t m)
{
    ssize_t len = strlen(cmd);
    if('\n' == cmd[len - 1]){
        cmd[len - 1] = 0;
    }
    printf("cmd:%s", cmd);

    if(strcmp(CMD_ENTER, cmd) == 0){
        _display_matrix(m);
    }else if(strcmp(CMD_QUIT, cmd) == 0){
        return 0;
    }

    return 1;
}


static  
void    
_display_matrix(matrix_t m)
{
    int index_row, index_col;
    ssize_t row_cnt, col_cnt;
    
    row_cnt = matrix_row_cnt(m);
    col_cnt = matrix_col_cnt(m);
   
    for(index_row = 0; index_row < row_cnt; index_row++){
        for(index_col = 0; index_col < col_cnt; index_col++){
            printf("%f", matrix_get(m, index_row, index_col));
        }
        printf("\n");
    }
}
