#ifndef TYPE_INCLUDE
#define TYPE_INCLUDE

#define T type_t


typedef struct T{
    const char *type;
}T; 


const extern T Matrix;
const extern T Vector;


#undef T
#endif //TYPE_INCLUDE
