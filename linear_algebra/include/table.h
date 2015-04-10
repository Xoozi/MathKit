#ifndef TABLE_INCLUDE
#define TABLE_INCLUDE

#define T table_t

#define KEY_LEN 128

typedef struct T *T;

extern T        table_new(int hint);
extern void     table_free(T *table);

extern int      table_length(T table);
extern void    *table_put(T table, const char *key, void *value);
extern void    *table_get(T table, const char *key);
extern void    *table_remove(T table, const char *key);

extern void     table_map(T table,
                        void (*apply)(const char *key, void **value, void *cl),
                        void *cl);

extern void   **table_to_array(T table, void *end);

#undef T

#endif /*TABLE_INCLUDE*/
