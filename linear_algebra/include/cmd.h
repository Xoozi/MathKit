#ifndef __INCLUDE_CMD__
#define __INCLUDE_CMD__

#define T cmd_t

typedef struct T *T;

extern  T       cmd_new          (char *cmd, int (*fun)(char *arg, void *cl));
extern  void    cmd_free         (T *cmd);
extern  int     cmd_cmp          (T cmd, char *cmd_str);
extern  int     cmd_run          (T cmd, char *arg, void *cl);

#undef T
#endif //__INCLUDE_CMD__
