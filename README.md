<!--
 * @encode: utf-8
 * @Date: 2025-08-20 21:11:49
 * @LastEditTime: 2025-08-29 13:52:59
 * @FilePath: /copy paste optimize/src/README.md
-->

### 简介

使用`ctrl+c`与`ctrl+v`复制粘贴文本或文件时,时常不知道是否是否复制成功,所以会多次按下`ctrl+c`

复制文件时,偶尔会有复制文件时,没有提示错误,但实际失败的情况

这个程序对以上情况进行了简单的解决

---
- 按下`ctrl+c`后会有提示

    提示1: 改变光标样式指定时间

    提示2: 改变光标样式,直到按下`ctrl+v`才恢复
    
    提示3: 响一下系统提示音
    
    以上均为可选

- 使用`ctrl+c ctrl+v`复制文件时,使用`sha256`校验是否复制完整

---

### 使用
在`config.yaml`可以直接编辑进行配置
也可以使用[setting.exe](https://github.com/tiandic/Minimalist-settings)进行图形化配置

改变的鼠标样式只有3种,可以自定义

`normal.cur`
`ibeam.cur`
`appstarting.cur`

将需要自定义的鼠标样式文件更名为以上的文件名,将其放在安装目录下即可

### 编译

#### 依赖
- [msys2](https://www.msys2.org/)
- [libyaml](https://github.com/yaml/libyaml)
- [python 3.12](https://www.python.org/downloads/)

```
git clone https://github.com/tiandic/copy-paste-optimize.git

cd copy-paste-optimize

./make.bat
```
生成的文件在`out`文件夹中,
`copyOptimize.exe`为主程序