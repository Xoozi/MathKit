#ifndef OBJECT_INCLUDE
#define OBJECT_INCLUDE


#define T obj_t


typedef struct T *T;

extern T        obj_new(type_t type, void *data);

extern void     obj_free(T *obj);
extern type_t   obj_type(T obj);

#undef T

#endif /*OBJECT_INCLUDE*/
