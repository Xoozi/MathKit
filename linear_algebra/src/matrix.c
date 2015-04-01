#include<stdlib.h>
#include<string.h>

#include"matrix.h"

#define T matrix_t

struct T{
    ssize_t row_cnt;
    ssize_t col_cnt;
    double *data;
    double *tmp;    /** 
                     为了便于行交换和缓存额外多分配了一行
                     作为临时空间，将临时空间起始处指针存放在
                     tmp中*/
};

/**
 * 辅助函数获取行起始位置
 */
static double * _get_rowstart(T matrix, ssize_t row);   

/**
 * 辅助函数将行复制到临时区
 */
static void     _copy_row(T matrix, ssize_t row);


T       
matrix_new      
(ssize_t row, ssize_t col)
{
    T ret_val;
    ret_val     = malloc(sizeof(*ret_val));

    ret_val->row_cnt    = row;
    ret_val->col_cnt    = col;
    ret_val->data       = malloc(row * col * sizeof(double));
    ret_val->tmp        = malloc(col * sizeof(double));
    return ret_val;
}

void    
matrix_free     
(T *matrix)
{
    free((*matrix)->data);
    free((*matrix)->tmp);
    free(*matrix);
}

ssize_t 
matrix_row_cnt
(T matrix)
{
    return matrix->row_cnt;
}

ssize_t 
matrix_col_cnt  
(T matrix)
{
    return matrix->col_cnt;
}

T       
matrix_set_row  
(T matrix, ssize_t row, double* cols)
{
    int index;
    ssize_t col_cnt;
    double *row_start;


    col_cnt = matrix->col_cnt;
    row_start = matrix->data + row * col_cnt;
    for(index = 0; index < col_cnt; index++){
        *(row_start+index) = *(cols+index);
    }

    if(index >= col_cnt)
        return NULL;
    else
        return matrix;
}

T       
matrix_exchange 
(T matrix, ssize_t row1, ssize_t row2)
{
    ssize_t row_step;
    double *row1_start, *row2_start;

    row_step    = matrix->col_cnt * sizeof(double);
    row1_start  = _get_rowstart(matrix, row1);
    row2_start  = _get_rowstart(matrix, row2);

    memcpy(matrix->tmp, row1_start, row_step);
    memcpy(row1_start, row2_start, row_step);
    memcpy(row2_start, matrix->tmp, row_step);
    
    return matrix;
}

T       
matrix_row_mul  
(T matrix, ssize_t row, double factor)
{
    int index;
    double *row_start;

    row_start   = _get_rowstart(matrix, row);
    for(index = 0; index < matrix->col_cnt; index++){
        *(row_start + index) = *(row_start + index) * factor; 
    }

    return matrix;
}

T       
matrix_row_dev
(T matrix, ssize_t row, double divisor)
{
    int index;
    double *row_start;

    row_start   = _get_rowstart(matrix, row);
    for(index = 0; index < matrix->col_cnt; index++){
        *(row_start + index) = *(row_start + index) / divisor;
    }
    return matrix;
}

T       
matrix_add_r2r      
(T matrix, ssize_t target, ssize_t other)
{
    int index;
    double *target_start, *other_start;
    target_start    = _get_rowstart(matrix, target);
    other_start     = _get_rowstart(matrix, other);

    for(index = 0; index < matrix->col_cnt; index++){
       *(target_start + index) = *(target_start + index)
           + *(other_start + index);
    }

    return matrix;
}

T
matrix_add_r2r_mul
(T matrix, ssize_t target, ssize_t other, double factor)
{
    int index;
    double *target_start, *tmp;
    target_start = _get_rowstart(matrix, target);
    tmp = matrix->tmp;

    _copy_row(matrix, other);

    for(index = 0; index < matrix->col_cnt; index++){
       *(target_start + index) = *(target_start + index)
           + (*(tmp + index) * factor);
    }

    return matrix;
}

T
matrix_add_r2r_dev
(T matrix, ssize_t target, ssize_t other, double divisor)
{
    int index;
    double *target_start, *tmp;
    target_start = _get_rowstart(matrix, target);
    tmp = matrix->tmp;

    _copy_row(matrix, other);

    for(index = 0; index < matrix->col_cnt; index++){
       *(target_start + index) = *(target_start + index)
           + (*(tmp + index) / divisor);
    }

    return matrix;
}

double
matrix_get
(T matrix, ssize_t row, ssize_t col)
{
    return *(matrix->data + row * matrix->col_cnt + col);
}


static
double *
_get_rowstart
(T matrix, ssize_t row)
{
    return matrix->data + row * matrix->col_cnt;
}

static 
void     
_copy_row(T matrix, ssize_t row)
{
    memcpy(matrix->tmp, 
            _get_rowstart(matrix, row),
            matrix->col_cnt * sizeof(double));
}
