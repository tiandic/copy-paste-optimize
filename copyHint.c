/*
 * @encode: utf-8
 * @Date: 2025-08-20 21:11:02
 * @LastEditTime: 2025-08-29 13:48:42
 * @FilePath: /copy paste optimize/src/copyHint.c
 */
#define OEMRESOURCE
#include <yaml.h>
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "copyHint.h"
#include "config.h"

int mouse_count = 0;

void add_strlist(strlist *l,char *str)
{
    strlist *node=l;
    while (node->next!=NULL)
        node = node->next;
    strlist *next_node = (strlist *)malloc(sizeof(strlist));
    char *next_str = (char *)malloc(strlen(str)+1);
    strncpy(next_str,str,strlen(str)+1);
    next_node->next=NULL;
    node->str=next_str;
    node->next=next_node;
}

void free_node(strlist *l)
{
    if (l->next!=NULL)
        free_node(l->next);
    if (l->next!=NULL)
        free(l->next);
    free(l->str);
}

void free_strlist(strlist *l)
{
    free_node(l);
    free(l);
}

bool getClipboardData(char *ClipboardData, size_t ClipboardDataSize)
{
    // 获取剪贴板内容
    // 成功则返回true,失败返回false
    HANDLE hData;
    char *pszText;

    if (!OpenClipboard(NULL))
        return false;
    if (((hData = GetClipboardData(CF_TEXT)) == NULL) || ((pszText = (char *)GlobalLock(hData)) == NULL))
    {
        CloseClipboard();
        return false;
    }
    memcpy(ClipboardData, pszText, ClipboardDataSize);
    GlobalUnlock(hData);
    CloseClipboard();
    return true;
}

void ReadClipboardFilePaths(strlist *fileNames) {
    HANDLE hData;
    HDROP hDrop;
    UINT pathLength;
    char* filePath;
    if (OpenClipboard(NULL) && IsClipboardFormatAvailable(CF_HDROP) &&
        (hData= GetClipboardData(CF_HDROP)) != NULL &&
        (hDrop = (HDROP)GlobalLock(hData))  != NULL)
    {
        for (UINT i = 0; i < DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); ++i) {
            pathLength = DragQueryFile(hDrop, i, NULL, 0);
            if ((filePath = (char*)malloc(pathLength + 1)) != NULL) {
                DragQueryFile(hDrop, i, filePath, pathLength + 1);
                add_strlist(fileNames,filePath);
                // printf("File path: %s\n", filePath);
                free(filePath);
            }
        }
        GlobalUnlock(hData);
    }
    CloseClipboard();
}

void run_verify_copy(const char *node_str, const char *target_dir)
{
    char cmd[1024*5];
    snprintf(cmd, sizeof(cmd), "./Verify_copy.exe \"%s\" \"%s\" y", node_str, target_dir);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcess 需要可写的命令行字符串
    char cmd_line[1024*5];
    strcpy(cmd_line, cmd);

    if (CreateProcess(
            NULL,       // 可执行文件由命令行解析
            cmd_line,   // 命令行
            NULL,       // 默认进程安全属性
            NULL,       // 默认线程安全属性
            FALSE,      // 不继承句柄
            0,          // 创建标志
            NULL,       // 使用父进程环境
            NULL,       // 使用父进程当前目录
            &si,        // STARTUPINFO 指针
            &pi))       // PROCESS_INFORMATION 指针
    {
        // 等待子进程结束
        // WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        printf("CreateProcess failed (%lu).\n", GetLastError());
    }
}

size_t getLine(FILE *f, char *line, size_t maxlen)
{
    int ch;
    size_t i = 0;

    while ((ch = fgetc(f)) != EOF)
    {
        if (ch == '\n' || i >= maxlen - 1)
            break;
        line[i++] = (char)ch;
    }

    line[i] = '\0';
    return i;
}

char* run_hidden(const char* cmd) {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hRead = NULL, hWrite = NULL;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char buffer[1024];
    DWORD dwRead;
    size_t total = 0, cap = 4096;

    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        return NULL;
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;   // 隐藏窗口
    si.hStdOutput = hWrite;
    si.hStdError  = hWrite;

    ZeroMemory(&pi, sizeof(pi));

    // 拼接成 cmd.exe /c <cmd>
    char cmdline[512];
    snprintf(cmdline, sizeof(cmdline), "cmd.exe /c %s", cmd);

    if (!CreateProcessA(
            NULL,
            cmdline,
            NULL,
            NULL,
            TRUE,   // 继承句柄
            0,
            NULL,
            NULL,
            &si,
            &pi))
    {
        CloseHandle(hRead);
        CloseHandle(hWrite);
        return NULL;
    }

    CloseHandle(hWrite); // 父进程不用写端
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    char *result = (char*)malloc(cap);
    if (!result) {
        CloseHandle(hRead);
        return NULL;
    }
    result[0] = '\0';

    while (ReadFile(hRead, buffer, sizeof(buffer), &dwRead, NULL) && dwRead > 0) {
        if (total + dwRead + 1 > cap) {
            cap *= 2;
            char *newbuf = (char*)realloc(result, cap);
            if (!newbuf) {
                free(result);
                CloseHandle(hRead);
                return NULL;
            }
            result = newbuf;
        }
        memcpy(result + total, buffer, dwRead);
        total += dwRead;
    }
    result[total] = '\0';

    CloseHandle(hRead);
    return result;
}

void rtrim(char *s) {
    int len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';  // 把末尾的空白字符替换成 \0
    }
}

void get_foreground_explorer_path(char *path,size_t path_size)
{
    char *output = run_hidden("get_explorer_dir.exe");
    strncpy(path,output,path_size);
    rtrim(path);
    free(output);
    // FILE *fp = _popen_hidden("get_explorer_dir.exe","r");
    // if (fp == NULL) {
    //     perror("popen failed");
    //     return;
    // }
    // getLine(fp,path,path_size);
    // _pclose_hidden(fp);
}

unsigned __stdcall verificationFile(void *a)
{
    strlist *head=(strlist *)malloc(sizeof(strlist));
    char target_dir[1024];
    strlist *node;
    head->str=NULL;
    head->next=NULL;
    ReadClipboardFilePaths(head);
    if (head->str==NULL)
        return 1;
    node=head;
    get_foreground_explorer_path(target_dir,sizeof(target_dir));
    while(node->next!=NULL)
    {
        run_verify_copy(node->str,target_dir);
        node = node->next;
    }
}


void recoverCursor()
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW; // 使用 wShowWindow
    si.wShowWindow = SW_HIDE;          // 隐藏窗口

    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA(
            "recoverCursor.exe", // 直接运行你的程序
            NULL,                 // 无参数
            NULL, NULL, FALSE, 0,
            NULL, NULL,
            &si, &pi))
    {
        // 成功后关闭句柄（让它在后台自己跑）
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void Sththread(func_v func)
{
    HANDLE hThread;
    unsigned threadId;

    // 创建线程
    hThread = (HANDLE)_beginthreadex(
        NULL,             // 默认安全属性
        0,                // 默认栈大小
        func,       // 线程函数
        NULL,       // 参数
        0,                // 默认标志（立即运行）
        &threadId         // 返回线程 ID
    );
    CloseHandle(hThread);
}


void easySetCursor(char *path,int target)
{
    HCURSOR hCursor = LoadCursorFromFileA(path);
    if (hCursor == NULL)
        MessageBox(NULL, "Cursor", "Cursor", MB_OK);
    SetSystemCursor(hCursor, target);
}

void setAllCursor()
{
    char normal_path[1024] = "normal.cur";
    char ibeam_path[1024] = "ibeam.cur";
    char appstarting_path[1024] = "appstarting.cur";

    if (access(normal_path, F_OK) != 0)
        strncpy(normal_path,"C:/Windows/Cursors/larrow.cur",sizeof(normal_path));
    if (access(ibeam_path, F_OK) != 0)
        strncpy(ibeam_path,"C:/Windows/Cursors/size4_l.cur",sizeof(ibeam_path));
    if (access(appstarting_path, F_OK) != 0)
        strncpy(appstarting_path,"C:/Windows/Cursors/size4_l.cur",sizeof(appstarting_path));

    easySetCursor(normal_path,OCR_NORMAL);
    easySetCursor(ibeam_path,OCR_IBEAM);
    easySetCursor(appstarting_path,OCR_APPSTARTING);
}

DWORD WINAPI changeCursor(LPVOID arg)
{
    // 用于hint函数
    // 通过改变光标来获取提示
    setAllCursor();
    Sleep(10000);
    mouse_count--;
    if (mouse_count > 0)
        return 0;
    recoverCursor();
    return 0;
}

void changeCursor_long()
{
    // 用于hint函数
    // 通过改变光标来获取提示
    setAllCursor();
}

void boop()
{
    // 用于hint函数
    // 播放系统默认的提示音
    MessageBeep(MB_ICONINFORMATION);
}

void hint()
{
    // 触发提示
    if (get_bool_config("config.yaml", "boop"))
        boop();
    if (get_bool_config("config.yaml", "mouse"))
    {
        mouse_count++;
        HANDLE hThread;
        DWORD threadId;

        hThread = CreateThread(
            NULL,       // 默认安全属性
            0,          // 默认栈大小
            changeCursor,     // 线程函数
            NULL,       // 参数
            0,          // 默认创建标志
            &threadId   // 返回线程 ID
        );
        CloseHandle(hThread);
    }
    if (get_bool_config("config.yaml", "long_mouse"))
        changeCursor_long();
}

void startListen()
{
    char lastClipboardData[BUFSIZE];
    char nowClipboardData[BUFSIZE];
    getClipboardData(lastClipboardData, sizeof(lastClipboardData));
    while (true)
    {
        if (getClipboardData(nowClipboardData, sizeof(nowClipboardData)))
        {
            if (memcmp(nowClipboardData, lastClipboardData, strlen(nowClipboardData) + 1) != 0)
            {
                memcpy(lastClipboardData, nowClipboardData, sizeof(nowClipboardData));
                hint();
            }
        }
        Sleep(100); // 暂停0.1秒,避免过高占用
    }
}