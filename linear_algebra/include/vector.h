#ifndef __INCLUDE_VECTOR__
#define __INCLUDE_VECTOR__

#define T vector_t

typedef struct T *T;


extern  T       vector_new          (ssize_t dimen);
extern  void    vector_free         (T *vector);

extern  ssize_t vector_dimen        (T vector);

extern  T       vector_set          (T vector, double* units);
extern  T       vector_set_at       (T vector, ssize_t pos, double value);
extern  T       vector_scalar_mul   (T vector, double scalar);
extern  double  vector_dot_mul      (T v1, T v2);
extern  T       vector_cross_mul    (T v1, T v2);
extern  double  vector_triple_mul   (T v1, T v2, T v3);
extern  T       vector_add          (T v_dest, T v_src);

extern  double  vector_get          (T vector, ssize_t pos);

#undef T
#endif //__INCLUDE_MATRIX__
