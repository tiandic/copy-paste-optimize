@echo off

mkdir out

windres app.rc -o out/app_res.o

gcc -static -shared .\copyHint.c .\config.c hookdll.c -o out/hookdll.dll -luser32 -lgdi32 -lyaml
gcc -static -mwindows .\main.c out/app_res.o -o out/copyOptimize.exe
gcc -static -mwindows .\recoverCursor.c -o out/recoverCursor.exe

pip install pyinstaller
pip install pywin32
pip install psutil

pyinstaller --noconsole -F Verify_copy.py --distpath out
pyinstaller -F get_explorer_dir.py --distpath out

copy config.yaml out

rmdir /s /q build
del out\app_res.o
del *.spec