#ifndef __INCLUDE_MATRIX__
#define __INCLUDE_MATRIX__

#include"vector.h"

#define T matrix_t

typedef struct T *T;


extern  T       matrix_new          (ssize_t row, ssize_t col);
extern  void    matrix_free         (T *matrix);

extern  ssize_t matrix_row_cnt      (T matrix);
extern  ssize_t matrix_col_cnt      (T matrix);

extern  T           matrix_set_row      (T matrix, ssize_t row, double* cols);
extern  T           matrix_exchange     (T matrix, ssize_t row1, ssize_t row2);
extern  T           matrix_row_mul      (T matrix, ssize_t row, double factor);
extern  T           matrix_row_dev      (T matrix, ssize_t row, double divisor);
extern  T           matrix_add_r2r      (T matrix, ssize_t target, ssize_t other);
extern  T           matrix_add_r2r_mul  (T matrix, ssize_t target, ssize_t other, double factor);
extern  T           matrix_add_r2r_dev  (T matrix, ssize_t target, ssize_t other, double divisor);
extern  vector_t    matrix_vector_mul   (T matrix, vector_t vector);

extern  double  matrix_get      (T matrix, ssize_t row, ssize_t col);

#undef T
#endif //__INCLUDE_MATRIX__
