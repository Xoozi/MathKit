#include<stdlib.h>
#include<string.h>
#include<assert.h>

#include"vector.h"

#define T vector_t

struct T{
    ssize_t dimen;
    double *data;
};


T       
vector_new          
(ssize_t dimen)
{
    T ret_val;
    ret_val     = malloc(sizeof(*ret_val));

    ret_val->dimen      = dimen;
    ret_val->data       = malloc(dimen * sizeof(double));
    return ret_val;
}


void    
vector_free         
(T *vector)
{
    free((*vector)->data);
    free(*vector);
}


ssize_t 
vector_dimen        
(T vector)
{
    return vector->dimen;
}


T       
vector_set          
(T vector, double* units)
{
    int index;
    ssize_t dimen;
    double *data;

    dimen = vector->dimen;
    data = vector->data;
    for(index = 0; index < dimen; index++){
        *(data+index) = *(units+index);
    }

    if(index >= dimen)
        return NULL;
    else
        return vector;
}



T       
vector_set_at       
(T vector, ssize_t pos, double value)
{
    ssize_t dimen;
    double *data;

    dimen = vector->dimen;
    data = vector->data;

    if(pos >= dimen)
        return NULL;

    *(data+pos) = value;

    return vector;
}



T       
vector_scalar_mul   
(T vector, double scalar)
{
    int index;
    ssize_t dimen;
    double *data;

    dimen = vector->dimen;
    data = vector->data;
    for(index = 0; index < dimen; index++){
        *(data+index) = (*(data+index))*scalar;
    }

    return vector;
}



double  
vector_dot_mul      
(T v1, T v2)
{
    int index;
    ssize_t dimen1, dimen2, max;
    double *data1, *data2;
    double ret_val, u1, u2;

    dimen1 = v1->dimen;
    dimen2 = v2->dimen;
    data1  = v1->data;
    data2  = v2->data;
    if(dimen1 >= dimen2){
        max = dimen1; 
    }else{
        max = dimen2;
    }

    ret_val = 0;
    for(index = 0; index < max; index++){
        if(index < dimen1)
            u1 = *(data1+index);
        else
            u1 = 0;
        if(index < dimen2)
            u2 = *(data2+index);
        else
            u2 = 0;

        ret_val += (u1 * u2);
    }

    return ret_val;
}



/**
 * 任意维度上的叉积我还得想一想
 */
T       
vector_cross_mul    
(T v1, T v2)
{
}



/**
 * 要用到叉积， 后面再搞
 */
double  
vector_triple_mul   
(T v1, T v2, T v3)
{
}



T       
vector_add          
(T v_dst, T v_src)
{

    int index;
    ssize_t dimen_dst, dimen_src;
    double *data_dst, *data_src;
    double ret_val, u1, u2;

    dimen_dst = v_dst->dimen;
    dimen_src = v_src->dimen;
    data_dst  = v_dst->data;
    data_src  = v_src->data;

    if(dimen_dst < dimen_src)
        return NULL;

    for(index = 0; index < dimen_dst; index++){
        if(index < dimen_src)
            u2 = *(data_src+index);
        else
            u2 = 0;

        *(data_dst+index) = *(data_dst+index) +  u2;
    }

    return v_dst;
}


double  
vector_get          
(T vector, ssize_t pos)
{
    ssize_t dimen;
    double *data;

    dimen = vector->dimen;
    data = vector->data;

    assert(pos < dimen);

    return *(data+pos);
}

