/*
 * @encode: utf-8
 * @Date: 2025-08-21 12:39:17
 * @LastEditTime: 2025-08-27 13:25:50
 * @FilePath: /copy paste optimize/src/main.c
 */
#include <windows.h>
#include <stdio.h>
#include "ch.h"

HMODULE hModule = NULL; // 用于存储加载的 DLL

// 处理 Ctrl+C 事件的回调函数
BOOL CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    case CTRL_C_EVENT:
        // 捕获 Ctrl+C 信号时卸载 DLL
        if (hModule != NULL)
        {
            FreeLibrary(hModule); // 卸载 DLL
            printf("DLL Unloaded Successfully.\n");
            exit(1);
        }
        return TRUE;
    default:
        return FALSE;
    }
}

int main()
{
    change_to_program_dir();
    // 设置控制台 Ctrl+C 事件的处理程序
    if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
    {
        MessageBox(NULL, "Failed to install CtrlHandler!", "Error", MB_OK);
        return 1;
    }

    // 加载 DLL
    HMODULE hModule = LoadLibrary("hookdll.dll");
    if (hModule == NULL)
    {
        DWORD errCode = GetLastError(); // 获取错误代码
        char* errMsg = NULL;

        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&errMsg,
            0,
            NULL
        );

        char buf[512];
        snprintf(buf, sizeof(buf), "Failed to load DLL!\nError code: %lu\nError message: %s", errCode, errMsg ? errMsg : "Unknown error");
        MessageBoxA(NULL, buf, "Error", MB_OK);

        if (errMsg) LocalFree(errMsg); // 释放 FormatMessage 分配的内存
        return 1;
    }

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 卸载 DLL
    FreeLibrary(hModule);
    return 0;
}
