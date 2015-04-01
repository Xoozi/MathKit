#ifndef __INCLUDE_HANDLER__
#define __INCLUDE_HANDLER__

#define T handler_t

#define CMD_LEN         1024
#define ERR_SUC             0
#define ERR_BAD_CMD         1
#define ERR_QUIT            -1

typedef struct T *T;

extern  T       handler_new     ();
extern  void    handler_free    (T *handler);
extern  void    handler_print   ();
extern  int     handler_run     (T handler, char *cmdline, matrix_t matrix);

#undef T
#endif //__INCLUDE_HANDLER__
