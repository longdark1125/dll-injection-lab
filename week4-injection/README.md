# Week 4-6: DLL 인젝션 프로젝트

## 개요
DLL 인젝션 기법을 이용하여 실행 중인 프로세스에 키로거를 주입하는 프로젝트

## 구성 파일

### dll/keylogger.dll
- 키로거 기능을 가진 DLL
- 주입되면 자동으로 키로깅 시작
- 로그 위치: `C:\keylog_dll.txt`

### injector/injector.exe
- DLL을 타겟 프로세스에 주입하는 프로그램
- CreateRemoteThread 기법 사용

## 실행 방법 (Windows)

### 1단계: 타겟 프로그램 실행
```cmd
notepad.exe
```

### 2단계: DLL 인젝션
```cmd
injector.exe notepad.exe C:\Users\[사용자명]\Downloads\week4-injection\dll\keylogger.dll
```

### 3단계: 확인
- 메모장에서 타이핑
- `C:\keylog_dll.txt` 파일 확인

## 기술적 설명

### DLL 인젝션 과정
1. 타겟 프로세스 ID 탐색
2. OpenProcess로 프로세스 핸들 획득
3. VirtualAllocEx로 메모리 할당
4. WriteProcessMemory로 DLL 경로 쓰기
5. CreateRemoteThread로 LoadLibraryA 실행
6. DLL이 로드되면서 DllMain 실행
7. 키로거 스레드 시작

### 3단계 연결
- Week 3: 키로거 개발
- Week 4-6: DLL 인젝션 (현재)
- Week 7: PDF 실행 시 자동 인젝션

