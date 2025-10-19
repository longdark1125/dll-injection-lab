#!/bin/bash

echo "================================"
echo "Compiling Keylogger Programs"
echo "================================"

# 키로거 컴파일
echo "[1/2] Compiling keylogger.c..."
x86_64-w64-mingw32-gcc keylogger.c -o keylogger.exe -luser32

# API 후킹 데모 컴파일
echo "[2/2] Compiling hook_demo.c..."
x86_64-w64-mingw32-gcc hook_demo.c -o hook_demo.exe -luser32

echo ""
echo "Compilation complete!"
echo ""
echo "Generated files:"
ls -lh *.exe

echo ""
echo "================================"
echo "Ready to test!"
echo "================================"
