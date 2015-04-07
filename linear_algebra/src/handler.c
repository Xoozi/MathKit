#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#include"table.h"
#include"type.h"
#include"object.h"
#include"matrix.h"
#include"handler.h"
#include"cmd.h"

#define T handler_t

#define CNT                 10

#define CMD_HELP            0
#define CMD_QUIT            1
#define CMD_DISPLAY         2 
#define CMD_ROW_MUL         3
#define CMD_ROW_DIV         4
#define CMD_ADD_ROW         5
#define CMD_ADD_ROW_MUL     6
#define CMD_ADD_ROW_DIV     7
#define CMD_EXCHANGE        8
#define CMD_SET_ROW         9

#define T_EOF 0
#define T_TEXT 1
#define T_NUM 2
#define T_NEWLINE 3

#define NAME_LEN            128

static char        last_obj_name[128];

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
static int      _display_matrix(char *name, matrix_t m);

static int      _help(char *arg, void *cl);
static int      _quit(char *arg, void *cl);
static int      _li(char *arg, void *cl);
static int      _display(char *arg, void *cl);
static int      _mat(char *arg, void *cl);
static int      _vec(char *arg, void *cl);
static int      _row_mul(char *arg, void *cl);
static int      _row_dev(char *arg, void *cl);
static int      _add_row(char *arg, void *cl);
static int      _add_row_mul(char *arg, void *cl);
static int      _add_row_dev(char *arg, void *cl);
static int      _exchange(char *arg, void *cl);
static int      _set_row(char *arg, void *cl);

T       
handler_new()
{
    T   ret_val;

    ret_val                 = malloc(sizeof(*ret_val));
    ret_val->cmd_list       = malloc(CNT * sizeof(*(ret_val->cmd_list)));
    
    ret_val->cmd_list[CMD_HELP]             = cmd_new("h", _help);
    ret_val->cmd_list[CMD_QUIT]             = cmd_new("q", _quit);
    ret_val->cmd_list[CMD_DISPLAY]          = cmd_new("d", _display);
    ret_val->cmd_list[CMD_ROW_MUL]          = cmd_new("rm", _row_mul);
    ret_val->cmd_list[CMD_ROW_DIV]          = cmd_new("rd", _row_dev);
    ret_val->cmd_list[CMD_ADD_ROW]          = cmd_new("ar", _add_row);
    ret_val->cmd_list[CMD_ADD_ROW_MUL]      = cmd_new("arm", _add_row_mul);
    ret_val->cmd_list[CMD_ADD_ROW_DIV]      = cmd_new("ard", _add_row_dev);
    ret_val->cmd_list[CMD_EXCHANGE]         = cmd_new("ex", _exchange);
    ret_val->cmd_list[CMD_SET_ROW]          = cmd_new("set", _set_row);

    last_obj_name[0] = 0;
    return ret_val;
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
    "   h                               (print this)\n"
    "   q                               (exit matkit)\n"
    "   li                              (list objects)\n"
    "   d     [name]                    (display object)\n"
    "   mat   name row col              (create a matrix with row & col if exist will update object)\n"
    "   vec   name len                  (create a vector with length if exist and so on)\n"
    "   rm    [name] row factor         (matrix: multiply factor to on row)\n"
    "   rd    [name] row divisor        (matrix: row divided by divisor)\n"
    "   ar    [name] row1 row2          (matrix: add row2 to row1, update row1'values)\n"
    "   arm   [name] row1 row2 factor   (matrix: multipy factor to row2 then add it to row1)\n"
    "   ard   [name] row1 row2 divisor  (matrix: row2 divided by divisor then add it to row1)\n"
    "   ex    [name] row1 row2          (matrix: exchange row1 and row2)\n"
    "   set   [name] row col1 col2...   (matrix: set row's cols)\n");

}


int     
handler_run     
(T handler, char *cmdline, table_t obj_table)
{
    int token, index;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = cmdline;

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return _display(NULL, obj_table);
    }else if(T_TEXT == token){
        for(index = 0; index < CNT; index++){
            cmd_t *c = (handler->cmd_list) + index;
            if(0 == cmd_cmp(handler->cmd_list[index], state.text)){

                return cmd_run(handler->cmd_list[index], state.ptr, obj_table);
            }
        }
        printf("unknown cmd:%s\n", state.text);
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
    /*ungetc('c', stdin);
    ungetc('o', stdin);
    ungetc('c', stdin);
    ungetc('k', stdin);
    return ERR_SUC;
    */
    return ERR_QUIT;
}

static
int
_display(char *arg, void *cl)
{
    table_t obj_list;
    type_t  type;
    obj_t   obj;
    char    *name;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_TEXT == token){
        name = state.text;
    }else if(T_NEWLINE == token && last_obj_name[0] != 0){
        name = last_obj_name; 
    }else{
        return ERR_BAD_CMD;
    }

    obj_list= cl;
    obj     = table_get(obj_list, name);
    
    if(null == obj){
        return ERR_NO_OBJ;
    }

    strncpy(last_obj_name, name, NAME_LEN);

    type    = obj_type(obj);
    if(Matrix == type){
        _display_matrix(name, obj_data(obj));
    }
    return ERR_SUC;
}

static 
int      
_li(char *arg, void *cl)
{
    table_t obj_list;
    type_t  type;
    obj_t   obj;
    void    **array;
    char    *name;
    int     i;

    obj_list = cl;

    array = table_to_array(obj_list, NULL);

    for(i = 0; array[i]; i+=2){
        name    = array[i];
        obj     = array[i+1];
        type    = obj_type(obj);

        if(Matrix == type){
            _display_matrix(name, obj_data(obj));
        }
    }
    free(array);
    return ERR_SUC;
}


static 
int      
_display_matrix(char *name, matrix_t m)
{

    int index_row, index_col;
    ssize_t row_cnt, col_cnt;

    row_cnt = matrix_row_cnt(m);
    col_cnt = matrix_col_cnt(m);
   
    printf("Matrix [%s]:\n", name);
    for(index_row = 0; index_row < row_cnt; index_row++){
        for(index_col = 0; index_col < col_cnt; index_col++){
            printf("%g\t", matrix_get(m, index_row, index_col));
        }
        printf("\n");
    }
}



static 
int      
_mat(char *arg, void *cl)
{
    table_t     obj_list;
    type_t      type;
    obj_t       obj;
    matrix_t    m;
    int         row, col;
    char        *name;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_TEXT == token){
        name = state.text;
    }else{
        return ERR_BAD_CMD;
    }


    token = _parse_cmd(&state);
    if(T_NUM == token){
        row = atoi(state.text);
    }else{
        return ERR_BAD_CMD;
    }

    token = _parse_cmd(&state);
    if(T_NUM == token){
        col = atoi(state.text);
    }else{
        return ERR_BAD_CMD;
    }

    obj_list= cl;
    m       = matrix_new(row, col);
    if(NULL == m){
        return ERR_CREATE;
    }

    table
    

    sncpy(last_obj_name, name, NAME_LEN);

    type    = obj_type(obj);
    if(Matrix == type){
        _display_matrix(name, obj_data(obj));
    }
    return ERR_SUC;
}



static 
int      
_vec(char *arg, void *cl)
{

}


static 
int      
_row_mul(char *arg, void *cl)
{
    matrix_t m = cl;
    ssize_t row;
    double factor;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        row = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        factor  = atof(state.text);
    }

    matrix_row_mul(m, row, factor);
    return ERR_SUC;
}

static 
int      
_row_dev(char *arg, void *cl)
{
    matrix_t m = cl;
    ssize_t row;
    double divisor;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        row = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        divisor  = atof(state.text);
    }

    matrix_row_dev(m, row, divisor);
    return ERR_SUC;
}
static 
int      
_add_row(char *arg, void *cl)
{
    matrix_t m = cl;
    ssize_t target, other;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        target = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        other = atoi(state.text);
    }

    matrix_add_r2r(m, target, other);
    return ERR_SUC;
}
static 
int      
_add_row_mul(char *arg, void *cl)
{
    matrix_t m = cl;
    ssize_t target, other;
    double factor;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        target = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        other = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        factor  = atof(state.text);
    }

    matrix_add_r2r_mul(m, target, other, factor);
    return ERR_SUC;
}
static 
int      
_add_row_dev(char *arg, void *cl)
{
    matrix_t m = cl;
    ssize_t target, other;
    double divisor;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        target = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        other = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        divisor  = atof(state.text);
    }

    matrix_add_r2r_dev(m, target, other, divisor);
    return ERR_SUC;
}
static 
int      
_exchange(char *arg, void *cl)
{
    matrix_t m = cl;
    ssize_t row1, row2;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        row1 = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        row2 = atoi(state.text);
    }

    matrix_exchange(m, row1, row2);
    return ERR_SUC;
}
static 
int      
_set_row(char *arg, void *cl)
{
    matrix_t m = cl;
    ssize_t row, col_cnt;
    col_cnt = matrix_col_cnt(m);
    double cols[col_cnt];
    int token, index;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_TEXT == token){
        row = atoi(state.text);
    }

    for(index = 0; index < col_cnt; index++){
        token = _parse_cmd(&state);
        if(T_NEWLINE == token){
            return ERR_BAD_CMD;
        }else if(T_TEXT == token){
            cols[index] = atof(state.text);
        }
    }

    matrix_set_row(m, row, cols);
    return ERR_SUC;
}

static 
int      
_parse_cmd(struct parse_state *state)
{
    int digit_count, alpha_count;
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
    if(0 != digit_count && 0 == alpha_count)
        return T_NUM;
    else
        return T_TEXT;
text:
    state->text = s = x;
    digit_count = 0;
    alpha_count = 0;
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
            if(isalpha(*x)){
                alpha_count += 1;
            }else if(isdigit(*x)){
                digit_count += 1;
            }
            *s++ = *x++;
        }
    }
    return T_EOF;
}
