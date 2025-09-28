@echo off
chcp 65001 >nul
echo ================================
echo DLL Injection Lab Starting...
echo ================================

echo Downloading Docker image...
docker pull longdark1125/dll-injection-lab:latest

echo Starting web development environment...
echo Please access http://localhost:8080 in your browser
echo.
docker run -it --rm -p 8080:8080 longdark1125/dll-injection-lab:latest bash -c "code-server --bind-addr 0.0.0.0:8080 --auth none /workspace"

pause
