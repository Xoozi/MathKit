#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>
#include<fcntl.h>

#include"table.h"
#include"type.h"
#include"object.h"
#include"matrix.h"
#include"vector.h"
#include"handler.h"
#include"cmd.h"
#include"cson.h"

#define T handler_t

#define CNT                 19

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
#define CMD_MAT             10
#define CMD_LI              11
#define CMD_DUMP            12
#define CMD_LOAD            13
#define CMD_VEC             14
#define CMD_VSET            15
#define CMD_VSET_AT         16
#define CMD_VSP             17
#define CMD_VADD            18

#define T_EOF 0
#define T_TEXT 1
#define T_NUM 2
#define T_NEWLINE 3


static char        _last_obj_name[KEY_LEN];
static char        *_default_prompt = "mt";

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
static int      _display_vector(char *name, vector_t v);
static void     _set_last_name(const char *name);
static void     _dump_vector(CSON *array, const char *name, vector_t v);
static void     _dump_matrix(CSON *array, const char *name, matrix_t m);
static obj_t    _get_select_obj(void *cl);
static matrix_t _get_select_matrix(void *cl);
static vector_t _get_select_vector(void *cl);

static int      _help(char *arg, void *cl);
static int      _quit(char *arg, void *cl);
static int      _li(char *arg, void *cl);
static int      _display(char *arg, void *cl);
static int      _dump(char *arg, void *cl);
static int      _load(char *arg, void *cl);
static int      _mat(char *arg, void *cl);

static int      _row_mul(char *arg, void *cl);
static int      _row_dev(char *arg, void *cl);
static int      _add_row(char *arg, void *cl);
static int      _add_row_mul(char *arg, void *cl);
static int      _add_row_dev(char *arg, void *cl);
static int      _exchange(char *arg, void *cl);
static int      _set_row(char *arg, void *cl);



static int      _vec(char *arg, void *cl);
static int      _vset(char *arg, void *cl);
static int      _vset_at(char *arg, void *cl);
static int      _vector_scalar_product(char *arg, void *cl);
static int      _vadd(char *arg, void *cl);

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
    ret_val->cmd_list[CMD_SET_ROW]          = cmd_new("mset", _set_row);
    ret_val->cmd_list[CMD_MAT]              = cmd_new("mat", _mat);
    ret_val->cmd_list[CMD_LI]               = cmd_new("li", _li);
    ret_val->cmd_list[CMD_DUMP]             = cmd_new("dump", _dump);
    ret_val->cmd_list[CMD_LOAD]             = cmd_new("load", _load);

    ret_val->cmd_list[CMD_VEC]              = cmd_new("vec", _vec);
    ret_val->cmd_list[CMD_VSET]             = cmd_new("vset", _vset);
    ret_val->cmd_list[CMD_VSET_AT]          = cmd_new("vset@", _vset_at);
    ret_val->cmd_list[CMD_VSP]              = cmd_new("vsp", _vector_scalar_product);
    ret_val->cmd_list[CMD_VADD]             = cmd_new("vadd", _vadd);
    _last_obj_name[0] = 0;
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
    "   dump  filename                  (dump objects to file)\n"
    "   load  filename                  (load objects from file)\n"

    "   Matrix:\n"
    "   mat   name row col              (create a matrix with row & col if exist will update object)\n"
    "   rm    row factor                (matrix: multiply factor to on row)\n"
    "   rd    row divisor               (matrix: row divided by divisor)\n"
    "   ar    row1 row2                 (matrix: add row2 to row1, update row1'values)\n"
    "   arm   row1 row2 factor          (matrix: multipy factor to row2 then add it to row1)\n"
    "   ard   row1 row2 divisor         (matrix: row2 divided by divisor then add it to row1)\n"
    "   ex    row1 row2                 (matrix: exchange row1 and row2)\n"
    "   mset  row col1 col2...          (matrix: set row's cols)\n"

    "   Vector:\n"
    "   vec   name dimen                (create a vector with dimension)\n"
    "   vset  unit1 unit2...            (vector: set units)\n"
    "   vset@ pos unit                  (vector: set unit at position)\n"
    "   vsp   scalar                    (vector: scalar product)\n"
    "   vadd  name                      (vector: add name's vector to current)\n"
    );


}

const char *  
handler_prompt   
(T handler)
{
    if(strlen(_last_obj_name) <= 0){
        return _default_prompt;
    }else{
        return _last_obj_name;
    }
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
void
_dump_matrix(CSON *array, const char *name, matrix_t m)
{
    int     index_row, index_col;
    int     row_cnt, col_cnt;
    CSON    *mson;
    CSON    *nums;

    mson = CSON_CreateObject();
    nums = CSON_CreateArray();
    row_cnt = matrix_row_cnt(m);
    col_cnt = matrix_col_cnt(m);
    CSON_AddStringToObject(mson, "name", name);
    CSON_AddStringToObject(mson, "type", "matrix");
    CSON_AddNumberToObject(mson, "row", row_cnt);
    CSON_AddNumberToObject(mson, "col", col_cnt);
    
    for(index_row = 0; index_row < row_cnt; index_row++){
        for(index_col = 0; index_col < col_cnt; index_col++){
            CSON_AddNumberToArray(nums,matrix_get(m, index_row, index_col));
        }
    }
    CSON_AddItemToObject(mson, "nums", nums); 
    CSON_AddItemToArray(array, mson);
}

static 
void
_dump_vector(CSON *array, const char *name, vector_t v)
{
    int     index;
    int     dimen;
    CSON    *mson;
    CSON    *nums;

    mson = CSON_CreateObject();
    nums = CSON_CreateArray();
    dimen= vector_dimen(v);
    CSON_AddStringToObject(mson, "name", name);
    CSON_AddStringToObject(mson, "type", "vector");
    CSON_AddNumberToObject(mson, "dimen", dimen);
    
    for(index = 0; index < dimen; index++){
        CSON_AddNumberToArray(nums,vector_get(v, index));
    }
    CSON_AddItemToObject(mson, "nums", nums); 
    CSON_AddItemToArray(array, mson);
}

static
void
_load_matrix(CSON *jm, table_t obj_list)
{
    int         index_row, index_col;
    int         row_cnt, col_cnt;
    int         offset;
    obj_t       obj;
    CSON        *name;
    CSON        *row;
    CSON        *col;
    CSON        *nums;
    CSON        *num;
    matrix_t    m;
    
    name = CSON_GetObjectItem(jm ,"name");
    row  = CSON_GetObjectItem(jm ,"row");
    col  = CSON_GetObjectItem(jm ,"col");
    nums = CSON_GetObjectItem(jm, "nums");

    row_cnt = row->valueint;
    col_cnt = col->valueint;

    m = matrix_new(row_cnt, col_cnt);

    double  row_content[col_cnt];
    for(index_row = 0; index_row < row_cnt; index_row++){
        offset = index_row * col_cnt;
        for(index_col = 0; index_col < col_cnt; index_col++){
            num = CSON_GetArrayItem(nums, offset + index_col);
            row_content[index_col] = num->valuedouble;
        }
        matrix_set_row(m, index_row, row_content);
    }
    obj = obj_new(&Matrix, m);
    table_put(obj_list, name->valuestring, obj);
}


static
void
_load_vector(CSON *jm, table_t obj_list)
{
    int         index;
    int         dimen_cnt;
    obj_t       obj;
    CSON        *name;
    CSON        *dimen;
    CSON        *nums;
    CSON        *num;
    vector_t    v;
    
    name    = CSON_GetObjectItem(jm ,"name");
    dimen   = CSON_GetObjectItem(jm ,"dimen");
    nums    = CSON_GetObjectItem(jm, "nums");

    dimen_cnt = dimen->valueint;

    v = vector_new(dimen_cnt);

    double  units[dimen_cnt];
    for(index = 0; index < dimen_cnt; index++){
        num = CSON_GetArrayItem(nums, index);
        units[index] = num->valuedouble;
    }
    vector_set(v, units);
    obj = obj_new(&Vector, v);
    table_put(obj_list, name->valuestring, obj);
}

static 
void
_load_object(CSON *array, int pos, table_t obj_list)
{
    CSON    *oson;
    CSON    *type;
    oson = CSON_GetArrayItem(array, pos);
    type = CSON_GetObjectItem(oson ,"type");
    if(0 == strcmp("matrix", type->valuestring)){
        _load_matrix(oson, obj_list);
    }else if(0 == strcmp("vector", type->valuestring)){
        _load_vector(oson, obj_list);
    }
}

static 
int      
_dump(char *arg, void *cl)
{
    int     ret_val;
    table_t obj_list;
    obj_t   obj;
    size_t  write_num;
    void    **array;
    char    *name;
    int     out_fd;
    int     open_flag;
    int     file_perm;
    int     i;
    int     token;
    CSON    *root;
    char    *dump;
    char    *file;
    struct parse_state state;

    ret_val = ERR_SUC;

    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_TEXT == token){
        file = state.text;
    }else{
        ret_val = ERR_BAD_CMD;
        goto EXIT_NO_RELEASE;
    }



    obj_list = cl;
    array = table_to_array(obj_list, NULL);

    root = CSON_CreateArray();
    for(i = 0; array[i]; i+=2){
        name    = array[i];
        obj     = array[i+1];

        if(obj_check_type(obj, &Matrix)){
            _dump_matrix(root, name, obj_data(obj));
        }else if(obj_check_type(obj, &Vector)){
            _dump_vector(root, name, obj_data(obj));
        }
    }
    dump = CSON_Print(root);
    printf("dump to :\t%s\n%s\n", file, dump);
    write_num = strlen(dump);
    open_flag = O_CREAT | O_WRONLY | O_TRUNC;
    file_perm = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    out_fd  = open(file, open_flag, file_perm);
    if(-1 == out_fd){
        ret_val = ERR_IO;
        goto EXIT_RELEASE;
    }
    if(write_num != write(out_fd, dump, write_num)){
        ret_val = ERR_IO;
        goto EXIT_RELEASE;
    }
    if(-1 == close(out_fd)){
        ret_val = ERR_IO;
        goto EXIT_RELEASE;
    }

EXIT_RELEASE:
    free(array);
    CSON_Delete(root);
    free(dump);

EXIT_NO_RELEASE:
    return ret_val;
}


static 
int      
_load(char *arg, void *cl)
{
    int     ret_val;
    size_t  size;
    int     cnt;
    int     in_fd;
    int     i;
    int     token;
    CSON    *root;
    char    *buf;
    char    *file;
    struct parse_state state;

    ret_val = ERR_SUC;

    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_TEXT == token){
        file = state.text;
    }else{
        ret_val = ERR_BAD_CMD;
        goto EXIT_NO_RELEASE;
    }


    in_fd  = open(file, O_RDONLY);
    if(-1 == in_fd){
        ret_val = ERR_IO;
        goto EXIT_NO_RELEASE;
    }
     
    if(-1 == (size = lseek(in_fd, 0, SEEK_END))){
        ret_val = ERR_IO;
        goto EXIT_NO_RELEASE;
    }
    buf = malloc(size); 
    if(-1 == lseek(in_fd, 0, SEEK_SET)){
        ret_val = ERR_IO;
        goto EXIT_RELEASE;
    }
    if(size != read(in_fd, buf, size)){
        ret_val = ERR_IO;
        goto EXIT_RELEASE;
    }
    if(-1 == close(in_fd)){
        ret_val = ERR_IO;
        goto EXIT_RELEASE;
    }

    root = CSON_Parse(buf);
    cnt = CSON_GetArraySize(root);
    for(i = 0; i < cnt; i++){
        _load_object(root, i, cl);
    }

EXIT_RELEASE:
    free(buf);
    CSON_Delete(root);

EXIT_NO_RELEASE:
    return ret_val;
}

static
int
_display(char *arg, void *cl)
{
    int     token;
    table_t obj_list;
    obj_t   obj;
    char    *name;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_TEXT == token){
        name = state.text;
    }else if(T_NEWLINE == token || T_EOF == token){
        name = _last_obj_name; 
    }else{
        return ERR_BAD_CMD;
    }

    obj_list= cl;
    obj     = table_get(obj_list, name);
    
    if(NULL == obj){
        return ERR_NO_OBJ;
    }

    _set_last_name(name);

    if(obj_check_type(obj, &Matrix)){
        _display_matrix(name, obj_data(obj));
    }else if(obj_check_type(obj, &Vector)){
        _display_vector(name, obj_data(obj));
    }
    return ERR_SUC;
}

static 
int      
_li(char *arg, void *cl)
{
    table_t obj_list;
    obj_t   obj;
    void    **array;
    char    *name;
    int     i;

    obj_list = cl;

    array = table_to_array(obj_list, NULL);

    for(i = 0; array[i]; i+=2){
        name    = array[i];
        obj     = array[i+1];

        if(obj_check_type(obj, &Matrix)){
            _display_matrix(name, obj_data(obj));
        }else if(obj_check_type(obj, &Vector)){
            _display_vector(name, obj_data(obj));
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
_display_vector(char *name, vector_t v)
{

    int index;
    ssize_t dimen;

    dimen = vector_dimen(v);
   
    printf("Vector [%s]:\n", name);
    for(index= 0; index < dimen; index++){
        printf("%g\t", vector_get(v, index));
        printf("\n");
    }
}



static 
int      
_mat(char *arg, void *cl)
{
    int         token;
    table_t     obj_list;
    type_t      *type;
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

    obj = obj_new(&Matrix, m);
    
    table_put(obj_list, name, obj);
    
    _set_last_name(name);

    return ERR_SUC;
}





static 
obj_t    
_get_select_obj(void *cl)
{
    table_t obj_list;
    obj_list= cl;
    return table_get(obj_list, _last_obj_name);
}

static 
matrix_t
_get_select_matrix(void *cl)
{
    matrix_t m ;
    obj_t    obj;
    obj = _get_select_obj(cl);
    if(NULL == obj){
        return NULL;    
    }if(obj_check_type(obj, &Matrix)){
        return obj_data(obj);
    }else{
        return NULL;
    }
}


static 
vector_t 
_get_select_vector(void *cl)
{
    vector_t v;
    obj_t    obj;
    obj = _get_select_obj(cl);
    if(NULL == obj){
        return NULL;    
    }if(obj_check_type(obj, &Vector)){
        return obj_data(obj);
    }else{
        return NULL;
    }
}

static 
int      
_row_mul(char *arg, void *cl)
{
    matrix_t m = _get_select_matrix(cl);
    if(NULL == m){
        return ERR_TYPE_MISSMATCH;
    }
    ssize_t row;
    double factor;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        row = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        factor  = atof(state.text);
    }

    matrix_row_mul(m, row, factor);
    return ERR_SUC;
}

static 
int      
_row_dev(char *arg, void *cl)
{
    matrix_t m = _get_select_matrix(cl);
    if(NULL == m){
        return ERR_TYPE_MISSMATCH;
    }
    ssize_t row;
    double divisor;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        row = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        divisor  = atof(state.text);
    }

    matrix_row_dev(m, row, divisor);
    return ERR_SUC;
}
static 
int      
_add_row(char *arg, void *cl)
{
    matrix_t m = _get_select_matrix(cl);
    if(NULL == m){
        return ERR_TYPE_MISSMATCH;
    }
    ssize_t target, other;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        target = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        other = atoi(state.text);
    }

    matrix_add_r2r(m, target, other);
    return ERR_SUC;
}
static 
int      
_add_row_mul(char *arg, void *cl)
{
    matrix_t m = _get_select_matrix(cl);
    if(NULL == m){
        return ERR_TYPE_MISSMATCH;
    }

    ssize_t target, other;
    double factor;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        target = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        other = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        factor  = atof(state.text);
    }

    matrix_add_r2r_mul(m, target, other, factor);
    return ERR_SUC;
}
static 
int      
_add_row_dev(char *arg, void *cl)
{
    matrix_t m = _get_select_matrix(cl);
    if(NULL == m){
        return ERR_TYPE_MISSMATCH;
    }

    ssize_t target, other;
    double divisor;
    int token;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        target = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        other = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        divisor  = atof(state.text);
    }

    matrix_add_r2r_dev(m, target, other, divisor);
    return ERR_SUC;
}
static 
int      
_exchange(char *arg, void *cl)
{
    matrix_t m = _get_select_matrix(cl);
    if(NULL == m){
        return ERR_TYPE_MISSMATCH;
    }

    ssize_t row1, row2;
    int token;
    struct parse_state state;


    state.nexttoken = 0;
    state.ptr = arg;
    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        row1 = atoi(state.text);
    }

    token = _parse_cmd(&state);
    if(T_NEWLINE == token){
        return ERR_BAD_CMD;
    }else if(T_NUM == token){
        row2 = atoi(state.text);
    }

    matrix_exchange(m, row1, row2);
    return ERR_SUC;
}
static 
int      
_set_row(char *arg, void *cl)
{
    matrix_t m = _get_select_matrix(cl);
    if(NULL == m){
        return ERR_TYPE_MISSMATCH;
    }

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
    }else if(T_NUM == token){
        row = atoi(state.text);
    }

    for(index = 0; index < col_cnt; index++){
        token = _parse_cmd(&state);
        if(T_NEWLINE == token){
            return ERR_BAD_CMD;
        }else if(T_NUM == token){
            cols[index] = atof(state.text);
        }
    }

    matrix_set_row(m, row, cols);
    return ERR_SUC;
}



static 
int      
_vec(char *arg, void *cl)
{
    int         token;
    table_t     obj_list;
    type_t      *type;
    obj_t       obj;
    vector_t    v;
    int         dimen;
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
        dimen = atoi(state.text);
    }else{
        return ERR_BAD_CMD;
    }


    obj_list= cl;
    v = vector_new(dimen);
    if(NULL == v){
        return ERR_CREATE;
    }

    obj = obj_new(&Vector, v);
    
    table_put(obj_list, name, obj);
    
    _set_last_name(name);

    return ERR_SUC;
}


static 
int      
_vset(char *arg, void *cl)
{
    vector_t v = _get_select_vector(cl);
    if(NULL == v){
        return ERR_TYPE_MISSMATCH;
    }

    ssize_t dimen;
    dimen = vector_dimen(v);
    double units[dimen];
    int token, index;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;

    for(index = 0; index < dimen; index++){
        token = _parse_cmd(&state);
        if(T_NUM == token){
            units[index] = atof(state.text);
        }else{
            return ERR_BAD_CMD;
        }
    }

    vector_set(v, units);
    return ERR_SUC;
}



static 
int      
_vset_at(char *arg, void *cl)
{
    vector_t v = _get_select_vector(cl);
    if(NULL == v){
        return ERR_TYPE_MISSMATCH;
    }

    ssize_t dimen, pos;
    dimen = vector_dimen(v);
    int token, index;
    double unit;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;

    token = _parse_cmd(&state);
    if(T_NUM == token){
        pos = atoi(state.text);
        if(pos >= dimen)
            return ERR_OUT_BOUND;
    }else{
        return ERR_BAD_CMD;
    }

    token = _parse_cmd(&state);
    if(T_NUM == token){
         unit = atof(state.text);
    }else{
        return ERR_BAD_CMD;
    }

    vector_set_at(v, pos, unit);
    return ERR_SUC;
}

static 
int      
_vadd(char *arg, void *cl)
{
    vector_t v = _get_select_vector(cl);
    if(NULL == v){
        return ERR_TYPE_MISSMATCH;
    }

    table_t obj_list;
    obj_t   obj;
    ssize_t dimen;
    dimen = vector_dimen(v);
    char *name;
    int token, index;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;

    token = _parse_cmd(&state);
    if(T_TEXT == token){
         name = state.text;
    }else{
        return ERR_BAD_CMD;
    }

    obj_list= cl;
    obj     = table_get(obj_list, name);
    if(NULL == obj){
        return ERR_NO_OBJ;
    }

    if(obj_check_type(obj, &Vector)){
        vector_add(v, obj_data(obj));
    }else{
        return ERR_TYPE_MISSMATCH;
    }

    return ERR_SUC;
}


static 
int      
_vector_scalar_product(char *arg, void *cl)
{
    vector_t v = _get_select_vector(cl);
    if(NULL == v){
        return ERR_TYPE_MISSMATCH;
    }

    ssize_t dimen;
    dimen = vector_dimen(v);
    double scalar;
    int token, index;
    struct parse_state state;
    state.nexttoken = 0;
    state.ptr = arg;

    token = _parse_cmd(&state);
    if(T_NUM == token){
         scalar = atof(state.text);
    }else{
        return ERR_BAD_CMD;
    }

    vector_scalar_mul(v, scalar);
    return ERR_SUC;
}

static 
int      
_parse_cmd(struct parse_state *state)
{
    int digit_count, alpha_count;
    char *x = state->ptr;
    char *s;

    if(NULL == state->ptr){
        return T_EOF;
    }

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


static 
void     
_set_last_name(const char *name)
{
    strncpy(_last_obj_name, name, KEY_LEN);
}
