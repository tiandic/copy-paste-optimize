#include <stdbool.h>
#define BUFSIZE 1024

#ifndef F_OK
#define F_OK 0   // 测试文件是否存在
#endif

#ifndef R_OK
#define R_OK 4   // 读权限
#endif

#ifndef W_OK
#define W_OK 2   // 写权限
#endif

#ifndef X_OK
#define X_OK 1   // 执行权限（Windows 上基本无意义）
#endif

typedef struct STRLIST
{
    char *str;
    struct STRLIST *next;
} strlist;

typedef unsigned (__stdcall *func_v)(void *);


bool getClipboardData(char *ClipboardData, size_t ClipboardDataSize);
void hint();
void startListen();
void recoverCursor();
void add_strlist(strlist *l,char *str);
void free_strlist(strlist *l);
unsigned __stdcall verificationFile(void *a);
void Sththread(func_v func);