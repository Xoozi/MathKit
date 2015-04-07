

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"type.h"
#include"object.h"

#define T obj_t

struct T{
    type_t  type;
    void    *data;
};



T
obj_new(type_t type, void *data)
{
    T ret_val;


    ret_val = malloc(sizeof(*ret_val));
    ret_val->type = type;
    ret_val->data = data;

    return ret_val;
}

void     
obj_free(T *obj)
{
    free(*obj);
}


type_t   
obj_type(T obj)
{
    return obj->type;
}

void *   
obj_data(T obj)
{
    return obj->data;
}
