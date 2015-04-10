#ifndef TYPE_INCLUDE
#define TYPE_INCLUDE

#define T type_t


#define EQ(x, y) (&x == &y)

typedef struct T{
    char *type;
}T; 



const extern T Matrix;
const extern T Vector;


#undef T
#endif //TYPE_INCLUDE
