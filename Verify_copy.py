# 验证复制是否完整
# Usage : python Verify_copy.py source_file target_dir2 need_Confirm
#         python Verify_copy.py source_dir target_dir2 need_Confirm
import os
import sys
import hashlib
import tkinter as tk
from tkinter import messagebox

def verify_parm():
    if sys.argv[3]!='y' and sys.argv[3]!='n':
        return False
    if os.path.isfile(sys.argv[1]) and os.path.isdir(sys.argv[2]):
        return True
    if os.path.isdir(sys.argv[1]) and os.path.isdir(sys.argv[2]):
        return True
    return False

def get_source_files():
    source_files=[]
    if os.path.isdir(sys.argv[1]):
        for root,_,files in os.walk(sys.argv[1]):
            for file in files:
                source_files.append(os.path.join(root,file))
    elif os.path.isfile(sys.argv[1]):
        source_files.append(sys.argv[1])
    return source_files

def get_sha256_python(file_path):
    sha256 = hashlib.sha256()
    
    block_size = 8192
    
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(block_size), b""):
            sha256.update(chunk)
    
    return sha256.hexdigest()

def show_verification_popup(src, dest):
    def on_start():
        print("开始验证...")
        root.destroy()

    def on_cancel():
        print("取消验证，退出程序")
        root.destroy()
        sys.exit(0)

    root = tk.Tk()
    root.title("验证")

    # 让 Label 自动换行
    label = tk.Label(root, text=f"是否开始验证?\n{src} -> {dest}",
                     wraplength=400, justify="left")  # 可调整 wraplength 控制换行
    label.pack(pady=10)

    frame = tk.Frame(root)
    frame.pack(pady=5)

    start_btn = tk.Button(frame, text="开始验证", width=12, command=on_start)
    start_btn.pack(side="left", padx=5)

    cancel_btn = tk.Button(frame, text="取消验证", width=12, command=on_cancel)
    cancel_btn.pack(side="right", padx=5)

    # 先更新一次，获取窗口实际大小
    root.update_idletasks()
    win_w = root.winfo_width()
    win_h = root.winfo_height()

    # 获取屏幕大小并居中
    screen_w = root.winfo_screenwidth()
    screen_h = root.winfo_screenheight()
    x = (screen_w - win_w) // 2
    y = (screen_h - win_h) // 2
    root.geometry(f"{win_w}x{win_h}+{x}+{y}")

    root.resizable(False, False)
    root.mainloop()
def show_invalid_path(path):
    # 创建主窗口并隐藏
    root = tk.Tk()
    root.withdraw()
    
    # 获取当前窗口句柄并设置为置顶
    root.attributes("-topmost", True)
    
    # 显示提示框
    messagebox.showinfo("提示", f"{path}\n无效!")
    
    # 退出程序时销毁窗口
    root.destroy()

def get_taget_path(source_path):
    relative_path = os.path.relpath(source_path,sys.argv[1])
    return os.path.join(sys.argv[2],os.path.basename(sys.argv[1]),relative_path)

def main():
    if not verify_parm():
        print("参数错误!")
        sys.exit(1)

    if sys.argv[3]=='y':
        show_verification_popup(sys.argv[1],sys.argv[2])

    # 获取源文件的所有路径
    source_files=get_source_files()

    for source_file_path in source_files:
        # 获取目标的路径
        target_file_path = get_taget_path(source_file_path)
        # 开始验证
        print(f"verify {target_file_path}...")
        if os.path.isfile(target_file_path):
            source_hash = get_sha256_python(source_file_path)
            target_hash = get_sha256_python(target_file_path)
            if source_hash !=target_hash:
                show_invalid_path(target_file_path+f"\nsource_hash: {source_hash}\ntarget_hash: {target_hash}")
        else:
            show_invalid_path(target_file_path)

main()