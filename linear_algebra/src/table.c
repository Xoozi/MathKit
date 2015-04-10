#include<limits.h>
#include<stddef.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include"table.h"

#define T table_t


struct T{
    int size;
    int length;
    int (*cmp)(const char *x, const char *y);
    unsigned long (*hash)(const char *key);
    unsigned long timestamp;

    struct binding{
        struct binding *link;
        char  key[KEY_LEN];
        void *value;
    }**buckets;
};

static int _cmpatom(const char *x, const char *y);
static unsigned long _hashatom(const char *key);

T
table_new(int hint)
{
    T table;
    int i;
    static int primes[] = {509, 509, 1021, 2053, 4093,
        8191, 16381, 32771, 65521, INT_MAX};

    assert(hint >= 0);

    for(i = 1; primes[i] < hint; i++)
        ;

    table = malloc(sizeof(*table) + 
            primes[i-1] * sizeof(table->buckets[0]));
    table->size = primes[i-1];
    table->buckets = (struct binding **) (table + 1);
    table->hash = _hashatom;
    table->cmp  = _cmpatom;

    for(i = 0; i < table->size; i++)
        table->buckets[i] = NULL;
    table->length = 0;
    table->timestamp = 0;
    return table;
}


void *
table_get(T table, const char *key)
{
    int i;
    struct binding *p;

    assert(table);
    assert(key);

    i = (*table->hash)(key) % table->size;
    for(p = table->buckets[i]; p; p = p->link)
        if(0 == (*table->cmp)(key, p->key))
            break;

    return p ? p->value : NULL;
}


void *
table_put(T table, const char *key, void *value)
{
    int i;
    struct binding *p;
    void *prev;

    assert(table);
    assert(key);

    i = (*table->hash)(key) % table->size;
    for(p = table->buckets[i]; p; p = p->link)
        if(0 == (*table->cmp)(key, p->key))
            break;

    if(NULL == p){
        p = malloc(sizeof(*p));
        strncpy(p->key, key, KEY_LEN);
        p->link = table->buckets[i];
        table->buckets[i] = p;
        table->length++;
        prev = NULL;
    }else
        prev = p->value;

    p->value = value;
    table->timestamp++;
    return prev;
}


int
table_length(T table)
{
    assert(table);
    return table->length;
}


void
table_map(T table,
            void (*apply)(const char *key, void **value, void *cl),
            void *cl)
{
    int i;
    unsigned long stamp;
    struct binding *p;

    assert(table);
    assert(apply);
    stamp = table->timestamp;

    for(i = 0; i < table->size; i++)
        for(p = table->buckets[i]; p; p = p->link){
            apply(p->key, &p->value, cl);
            assert(stamp == table->timestamp);
        }
}


void *
table_remove(T table, const char *key)
{
    int i;
    struct binding *p, **pp;

    assert(table);
    assert(key);

    table->timestamp++;
    i = (*table->hash)(key) % table->size;

    for(pp = &table->buckets[i]; *pp; pp = &(*pp)->link)
        if(0 == (*table->cmp)(key, (*pp)->key)){
            p = *pp;
            void *value = p->value;
            *pp = p->link;
            free(p);
            table->length--;
            return value;
        }

    return NULL;
}



void **
table_to_array(T table, void *end)
{
    int i, j = 0;

    void **array;
    struct binding *p;

    assert(table);
    array = malloc((2 * table->length + 1) * sizeof(*array));
    for(i = 0; i < table->size; i++)
        for(p = table->buckets[i]; p; p = p->link){
            array[j++] = (void *)p->key;
            array[j++] = p->value;
        }

    array[j] = end;

    return array;
}


void
table_free(T *table)
{
    int i;
    struct binding *p, *q;

    assert(table && *table);

    if((*table)->length > 0){
        for(i = 0; i < (*table)->size; i++)
            for(p = (*table)->buckets[i]; p; p = q){
                q = p->link;
                free(p);
            }
    }

    free(*table);
}





static
int 
_cmpatom(const char *x, const char *y)
{
    return strcmp(x, y);
}

static
unsigned long
_hashatom(const char *key)
{
    unsigned long hash = 0;
    unsigned x = 0;

    while(*key){
        hash = (hash << 4) + (* key++);
        if((x = hash & 0xF0000000L) != 0){
            hash ^= (x >> 24);
            hash &= ~x;
        }
    }
    return (unsigned long) key >> 2;
}
