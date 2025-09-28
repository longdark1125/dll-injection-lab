#!/bin/bash

echo "================================"
echo "DLL Injection Lab 실행 중..."
echo "================================"

echo "Docker 이미지 다운로드 중..."
docker pull longdark1125/dll-injection-lab:latest

echo "웹 개발환경 시작 중..."
echo "잠시 후 브라우저에서 http://localhost:8080 에 접속하세요"
echo

docker run -it --rm -p 8080:8080 longdark1125/dll-injection-lab:latest bash -c "code-server --bind-addr 0.0.0.0:8080 --auth none /workspace"
