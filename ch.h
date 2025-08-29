#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#define chdir _chdir
#define getcwd _getcwd


int change_to_program_dir() {
    char exe_path[PATH_MAX];
    
    // 获取可执行文件的路径
        // Windows 系统
        DWORD len = GetModuleFileNameA(NULL, exe_path, PATH_MAX);
        if (len == 0 || len == PATH_MAX) {
            perror("GetModuleFileName failed");
            return -1;
        }
    
    // 提取目录部分（去掉文件名）
        // Windows 路径处理
        char drive[_MAX_DRIVE], dir[_MAX_DIR];
        if (_splitpath_s(exe_path, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0) != 0) {
            perror("_splitpath_s failed");
            return -1;
        }
        
        // 重新组合路径（只保留驱动器和目录）
        char full_dir[PATH_MAX];
        if (_makepath_s(full_dir, PATH_MAX, drive, dir, NULL, NULL) != 0) {
            perror("_makepath_s failed");
            return -1;
        }
        
        // 切换到程序所在目录
        if (chdir(full_dir) != 0) {
            perror("chdir failed");
            return -1;
        }

    
    // 验证切换是否成功（可选）
    char cwd[PATH_MAX];
    if (getcwd(cwd, PATH_MAX) != NULL) {
        printf("成功切换到程序目录: %s\n", cwd);
    } else {
        perror("getcwd failed");
        return -1;
    }
    
    return 0;
}
