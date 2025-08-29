'''
encode: utf-8
Date: 2025-08-27 11:32:49
LastEditTime: 2025-08-29 10:56:01
FilePath: /copy paste optimize/src/get_explorer_dir.py
'''
'''
encode: utf-8
Date: 2025-08-27 11:22:39
LastEditTime: 2025-08-28 19:26:39
FilePath: /copy paste optimize/src/get_explorer_dir.py
'''
import win32com.client
import win32gui
import win32con
import win32process
import psutil

def get_top_explorer_hwnd():
    hwnd = win32gui.GetTopWindow(None)  # 获取Z序最顶层的窗口
    explorer_hwnd = None

    while hwnd:
        # 获取窗口对应的进程ID
        _, pid = win32process.GetWindowThreadProcessId(hwnd)
        try:
            process = psutil.Process(pid)
            exe_name = process.name().lower()
        except psutil.NoSuchProcess:
            exe_name = ""

        # 判断是否为 explorer.exe（资源管理器）
        class_name = win32gui.GetClassName(hwnd)
        if exe_name == "explorer.exe" and class_name == "CabinetWClass":
            explorer_hwnd = hwnd
            break

        # 获取下一个窗口（按Z序往下）
        hwnd = win32gui.GetWindow(hwnd, win32con.GW_HWNDNEXT)

    return explorer_hwnd


def get_foreground_explorer_path():
    """获取前台资源管理器路径"""
    foreground_hwnd = get_top_explorer_hwnd()
    shell = win32com.client.Dispatch("Shell.Application")
    windows = shell.Windows()
    
    for window in windows:
        try:
            if window.HWND == foreground_hwnd and hasattr(window, "LocationURL"):
                url = window.LocationURL
                if url.startswith("file:///"):
                    return url[8:]  # 移除 "file:///"
                else:
                    return url.replace("file://", "")
        except:
            continue
    
    return ""

print(get_foreground_explorer_path())