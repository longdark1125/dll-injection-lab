# 7주차 DLL 인젝션 실습 환경
FROM ubuntu:22.04

# 환경 변수 설정 (설치 시 대화형 방지)
ENV DEBIAN_FRONTEND=noninteractive

# 기본 패키지 설치
RUN apt-get update && apt-get install -y \
    # 기본 도구
    curl wget git vim nano tree \
    # 개발 도구  
    gcc g++ make cmake \
    # Windows 크로스 컴파일
    gcc-mingw-w64 g++-mingw-w64 \
    # Wine (Windows 프로그램 실행)
    wine \
    # Python 개발
    python3 python3-pip python3-dev \
    # 네트워크 도구
    netcat-openbsd nmap \
    # 웹 개발환경용
    nodejs npm \
    && rm -rf /var/lib/apt/lists/*

# Python 패키지 설치
RUN pip3 install \
    psutil \
    pefile \
    capstone \
    requests

# 작업 디렉토리 설정
WORKDIR /workspace

# VS Code Server 설치 (웹 기반 개발환경)
RUN curl -fsSL https://code-server.dev/install.sh | sh

# 포트 노출
EXPOSE 8080

# 시작 명령
CMD ["bash"]
