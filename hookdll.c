/*
 * @encode: utf-8
 * @Date: 2025-08-24 16:13:50
 * @LastEditTime: 2025-08-28 18:17:46
 * @FilePath: /copy paste optimize/src/hookdll.c
 */
#include <windows.h>
#include <stdbool.h>
#include <io.h>
#include "copyHint.h"
#include "config.h"

HHOOK hHook = NULL;
char lastClipboardData[BUFSIZE];
char nowClipboardData[BUFSIZE];
bool is_ctrl_c=false;

// 钩子回调函数
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        // 获取按键信息
        KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;

        // 检测是否按下了 Ctrl+C
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&
            // 检查是否按下 Ctrl + C
            ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && pKeyBoard->vkCode == 0x43) &&
            (getClipboardData(nowClipboardData, sizeof(nowClipboardData))) &&
            (memcmp(nowClipboardData, lastClipboardData, strlen(nowClipboardData) + 1) != 0))
        {
            is_ctrl_c = true;
            // MessageBox(NULL, "Ctrl+C Detected!", "Hook", MB_OK);
            memcpy(lastClipboardData, nowClipboardData, sizeof(nowClipboardData));
            hint();
        }
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&
            // 检查是否按下 Ctrl + V
            ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && pKeyBoard->vkCode == 0x56))
        {
            if(get_bool_config("config.yaml","long_mouse"))
                recoverCursor();
            if (get_bool_config("config.yaml","Verify"))
                Sththread(verificationFile);
                // verificationFile();
            is_ctrl_c = false;
        }
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&
        // 检查是否按下 Ctrl + X
        ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && pKeyBoard->vkCode == 0x58))
        {
            is_ctrl_c = false;
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

// DLL入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        getClipboardData(lastClipboardData, sizeof(lastClipboardData));
        // 安装钩子
        if ((hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hModule, 0)) == NULL)
        {
            MessageBox(NULL, "Failed to install hook!", "Error", MB_OK);
            return FALSE;
        }
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        // 移除钩子
        if (hHook != NULL)
            UnhookWindowsHookEx(hHook);
    }
    return TRUE;
}
