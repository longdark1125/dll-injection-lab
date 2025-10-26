# Week 5: 자동 DLL 인젝션 모니터

## 개요
Adobe Acrobat 실행을 자동으로 감지하여 키로거 DLL을 자동 주입하는 프로그램

## 구성 파일
- **process_monitor.exe**: 프로세스 모니터링 및 자동 인젝션

## 동작 원리
1. 1초마다 프로세스 목록 스캔
2. 새로운 msedge.exe 프로세스 감지
3. 2초 대기 (프로세스 안정화)
4. keylogger.dll 자동 주입
5. 키로깅 시작

## 실행 방법 (Windows)

### 1단계: 모니터 실행
```cmd
process_monitor.exe
```

"Monitoring... Press Ctrl+C to stop" 메시지가 나타나면 대기 중

### 2단계: PDF 파일 열기
- 아무 PDF 파일 더블클릭
- msedge로 열림

### 3단계: 자동 인젝션 확인
콘솔에 다음 메시지가 나타남:
```
[!] NEW TARGET DETECTED!
[*] Process: msedge.exe (PID: ...)
[+] Injecting into PID: ...
[+] DLL injected successfully!
[SUCCESS] Keylogger activated!
```

### 4단계: 테스트
- msedge에서 Ctrl+F (찾기) 또는 메모 입력
- C:\keylog_dll.txt 파일 확인

## 3단계 진화 과정

### Week 3-4: 수동 인젝션
```
메모장 실행 → injector.exe 수동 실행 → 인젝션
```

### Week 5: 자동 인젝션 (현재)
```
process_monitor.exe 실행 → PDF 열기 → 자동 감지 → 자동 인젝션
```

### Week 6-7: 완전 자동화 (예정)
```
시스템 부팅 → 서비스 자동 시작 → PDF 열기 → 자동 인젝션 (사용자 모르게)
```

## 기술적 특징
- CreateToolhelp32Snapshot으로 프로세스 실시간 모니터링
- 중복 인젝션 방지 (프로세스 ID 추적)
- 프로세스 안정화 대기 (Sleep 2초)
- Week 4의 인젝션 코드 재사용
