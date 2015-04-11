#ifndef __INCLUDE_HANDLER__
#define __INCLUDE_HANDLER__
#include"table.h"

#define T handler_t

#define CMD_LEN         1024
#define ERR_SUC             0
#define ERR_BAD_CMD         1
#define ERR_NO_OBJ          2
#define ERR_CREATE          3
#define ERR_TYPE_MISSMATCH  4
#define ERR_IO              5
#define ERR_QUIT            -1

typedef struct T *T;

extern  T           handler_new     ();
extern  void        handler_free    (T *handler);
extern  void        handler_print   ();
extern  int         handler_run     (T handler, char *cmdline, table_t obj_table);
extern  const char *handler_prompt  (T handler);

#undef T
#endif //__INCLUDE_HANDLER__
