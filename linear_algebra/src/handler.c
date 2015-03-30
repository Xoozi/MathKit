#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include"handler.h"
#include"cmd.h"

#define T handler_t

#define CNT                 9

#define CMD_HELP            0
#define CMD_QUIT            1
#define CMD_DISPLAY         2 
#define CMD_ROW_MUL         3
#define CMD_ROW_DIV         4
#define CMD_ADD_ROW         5
#define CMD_ADD_ROW_MUL     6
#define CMD_ADD_ROW_DIV     7
#define CMD_EXCHANGE        8

#define T_EOF 0
#define T_TEXT 1
#define T_NEWLINE 2


struct T{
    cmd_t   *cmd_list;
};

struct parse_state
{
    char *ptr;
    char *text;
    int nexttoken;
};

static int      _parse_cmd(struct parse_state *state);

static int      _help(char *arg, void *cl);
static int      _quit(char *arg, void *cl);
static int      _display(char *arg, void *cl);
static int      _row_mul(char *arg, void *cl);
static int      _row_dev(char *arg, void *cl);
static int      _add_row(char *arg, void *cl);
static int      _add_row_mul(char *arg, void *cl);
static int      _add_row_dev(char *arg, void *cl);
static int      _exchange(char *arg, void *cl);

T       
handler_new()
{
    T   ret_val;

    ret_val                 = malloc(sizeof(*ret_val));
    ret_val->cmd_list       = malloc(CNT * sizeof(cmd_t));
    
    ret_val->cmd_list[CMD_HELP]             = cmd_new("h", _help);
    ret_val->cmd_list[CMD_QUIT]             = cmd_new("q", _quit);
    ret_val->cmd_list[CMD_DISPLAY]          = cmd_new("d", _display);
    ret_val->cmd_list[CMD_ROW_MUL]          = cmd_new("rm", _row_mul);
    ret_val->cmd_list[CMD_ROW_DIV]          = cmd_new("rd", _row_dev);
    ret_val->cmd_list[CMD_ADD_ROW]          = cmd_new("ar", _add_row);
    ret_val->cmd_list[CMD_ADD_ROW_MUL]      = cmd_new("arm", _add_row_mul);
    ret_val->cmd_list[CMD_ADD_ROW_DIV]      = cmd_new("ard", _add_row_dev);
    ret_val->cmd_list[CMD_EXCHANGE]         = cmd_new("ex", _exchange);
}


void    
handler_free(T *handler)
{
    int index;
    for(index = 0; index < CNT; index++){
        cmd_free(&((*handler)->cmd_list[index]));
    }
    free((*handler)->cmd_list);
    free(*handler);
}


void    
handler_print()
{
    int index;
    printf("commands:\n"
    "   h                      (print this)\n"
    "   q                      (exit matkit)\n"
    "   d                      (display matrix)\n"
    "   rm    row factor       (multiply factor to on row)\n"
    "   rd    row divisor      (row divided by divisor)\n"
    "   ar    row1 row2        (add row2 to row1, update row1'values)\n"
    "   arm   row1 row2 factor (multipy factor to row2 then add it to row1)\n"
    "   ard   row1 row2 divisor(row2 divided by divisor then add it to row1)\n"
    "   ex    row1 row2        (exchange row1 and row2)\n");

}


int     
handler_run     
(T handler, char *cmdline)
{
    int token, index;
    struct parse_state state;
    state.ptr = cmdline;

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        _display(NULL, NULL);
    }else if(T_TEXT == token){
        for(index = 0; index < CNT; index++){
            if(0 == cmd_cmp(handler->cmd_list[index], state.text))
        }
    }
    return ERR_BAD_CMD;
}

static 
int      
_help(char *arg, void *cl)
{
    handler_print();
}

static
int      
_quit(char *arg, void *cl)
{
    return ERR_QUIT;
}

static 
int      
_display(char *arg, void *cl)
{
    return ERR_SUC;
}

static 
int      
_row_mul(char *arg, void *cl)
{
    return ERR_SUC;
}

static 
int      
_row_dev(char *arg, void *cl)
{
    return ERR_SUC;
}
static 
int      
_add_row(char *arg, void *cl)
{
    return ERR_SUC;
}
static 
int      
_add_row_mul(char *arg, void *cl)
{
    return ERR_SUC;
}
static 
int      
_add_row_dev(char *arg, void *cl)
{
    return ERR_SUC;
}
static 
int      
_exchange(char *arg, void *cl)
{
    return ERR_SUC;
}

static 
int      
_parse_cmd(struct parse_state *state)
{

    char *x = state->ptr;
    char *s;

    if (state->nexttoken) {
        int t = state->nexttoken;
        state->nexttoken = 0;
        return t;
    }

    for (;;) {
        switch (*x) {
        case 0:
            state->ptr = x;
            return T_EOF;
        case '\n':
            x++;
            state->ptr = x;
            return T_NEWLINE;
        case ' ':
        case '\t':
        case '\r':
            x++;
            continue;
        default:
            goto text;
        }
    }

textdone:
    state->ptr = x;
    *s = 0;
    return T_TEXT;
text:
    state->text = s = x;
textresume:
    for (;;) {
        switch (*x) {
        case 0:
            goto textdone;
        case ' ':
        case '\t':
        case '\r':
            x++;
            goto textdone;
        case '\n':
            state->nexttoken = T_NEWLINE;
            x++;
            goto textdone;
        default:
            *s++ = *x++;
        }
    }
    return T_EOF;
}
