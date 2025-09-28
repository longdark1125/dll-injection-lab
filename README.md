DLL 인젝션 및 코드 인젝션 실습용 Docker 컨테이너 환경
이 프로젝트는 7주차에 걸친 DLL 인젝션 실습을 위한 통합 개발환경입니다.
웹 기반 VS Code 환경에서 Windows 프로그램 개발, DLL 생성, 인젝션 기법 등을 실습할 수 있습니다.
##  빠른 시작

### 1. 저장소 클론
```bash
git clone https://github.com/longdark1125/dll-injection-lab.git
cd dll-injection-lab
### 2.컨테이너 빌드
sudo docker build -t dll-injection-lab .
### 3.개발환경 실행
sudo docker run -it --rm -p 8080:8080 dll-injection-lab bash -c "code-server --bind-addr 0.0.0.0:8080 --auth none /workspace"
### 4.웹브라우저 접속http://localhost:8080
포함된 도구들

Ubuntu 22.04 - 안정적인 Linux 환경
Python 3.10 - 스크립팅 및 자동화
MinGW-w64 - Windows 프로그램 크로스 컴파일
Wine - Windows 프로그램 실행 환경
VS Code Server - 웹 기반 통합 개발환경
GCC/G++ - C/C++ 컴파일러
프로젝트 구조
dll-injection-lab/
├── Dockerfile              # 컨테이너 설정
├── README.md               # 프로젝트 문서
├── .gitignore             # Git 제외 파일
├── week3                  # 3주차 개발 실습
├── week4-6                # DLL 인젝션 실습
└── week7                  # 악성코드 분석 실습
