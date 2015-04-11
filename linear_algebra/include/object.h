#ifndef OBJECT_INCLUDE
#define OBJECT_INCLUDE


#define T obj_t


typedef struct T *T;

extern T                obj_new(const type_t *type, void *data);

extern void             obj_free(T *obj);
extern const type_t    *obj_type(T obj);
extern void            *obj_data(T obj); 
extern int              obj_check_type(T obj, const type_t *type);

#undef T

#endif /*OBJECT_INCLUDE*/
