# Week 5: 자동 DLL 인젝션 완성

## 개요
프로세스 실행을 자동으로 감지하여 키로거 DLL을 자동 주입하는 시스템

## 완성된 기능
-  프로세스 자동 감지 및 모니터링
-  자동 DLL 인젝션
-  폴링 방식 키로거 (샌드박스 대응)
-  중복 인젝션 방지

## 구성 파일
- **process_monitor.exe**: 프로세스 모니터 및 자동 인젝터
- **keylogger_polling.dll**: GetAsyncKeyState 기반 폴링 키로거

## 동작 원리

### 1. 프로세스 모니터링
```
1초마다 프로세스 목록 스캔
→ 새로운 notepad.exe 감지
→ 중복 체크 (이미 주입한 PID 확인)
→ 새 프로세스면 인젝션 진행
```

### 2. 자동 DLL 인젝션
```
OpenProcess (프로세스 핸들 획득)
→ VirtualAllocEx (메모리 할당)
→ WriteProcessMemory (DLL 경로 쓰기)
→ CreateRemoteThread (LoadLibraryA 실행)
→ DLL 로드 완료
```

### 3. 폴링 방식 키로깅
```
DLL 로드됨
→ 폴링 스레드 시작
→ 5ms마다 모든 키 상태 체크 (GetAsyncKeyState)
→ 새로 눌린 키 감지
→ C:\keylog_dll.txt에 기록
```

## 실행 방법 (Windows)

### 1단계: 모니터 실행
```cmd
process_monitor.exe
```

콘솔 출력:
```
=================================
Edge Browser Auto Injector
=================================
Target: notepad.exe
Monitoring... Press Ctrl+C to stop
```

### 2단계: 메모장 실행
- 시작 메뉴 → "메모장" 검색 → 실행
- 또는 `Win+R` → `notepad` → Enter

### 3단계: 자동 인젝션 확인
콘솔에 자동으로 표시:
```
[!] NEW EDGE PROCESS DETECTED!
[*] Process: notepad.exe (PID: 12345)
[+] DLL injected successfully!
[SUCCESS] Keylogger activated!
```

### 4단계: 테스트
- 메모장에서 타이핑
- 아무 내용이나 입력

### 5단계: 로그 확인
```cmd
notepad C:\keylog_dll.txt
```

예상 결과:
```
=== Polling Keylogger Started ===
hello world[ENTER]
this is a test[ENTER]
```

## 기술적 특징

### 폴링 방식 키로거의 장점
- **샌드박스 대응**: SetWindowsHookEx 차단을 우회
- **직접 확인**: GetAsyncKeyState로 키 상태 직접 조회
- **안정성**: Windows 알림 시스템에 의존하지 않음

### 한계와 학습 내용
**시도한 것:**
- Adobe Acrobat 타겟 ❌ (샌드박스)
- Edge 브라우저 타겟 ❌ (샌드박스 + API 필터링)
- SetWindowsHookEx 방식 ❌
- GetAsyncKeyState 폴링 (메모장 ✅, 브라우저 ❌)

**배운 것:**
- 현대 브라우저의 샌드박스 보호 메커니즘
- 사용자 모드 후킹의 한계
- 샌드박스 우회를 위한 다양한 기법
- 커널 레벨 접근의 필요성

## 3단계 진화 과정

### Week 3-4: 수동 인젝션
```
사용자: 메모장 실행
사용자: injector.exe 수동 실행
결과: 키로깅
```

### Week 5: 자동 인젝션 (현재) 
```
사용자: process_monitor.exe 실행
사용자: 메모장 실행
자동: 감지 및 인젝션
결과: 키로깅
```

### Week 6-7: 완전 자동화 (예정)
```
시스템: 부팅 시 서비스 자동 시작
사용자: 메모장 실행
자동: 백그라운드 인젝션
결과: 사용자 모르게 키로깅
```

## 샌드박스 우회 연구

### 시도한 방법들
1. **SetWindowsHookEx (WH_KEYBOARD_LL)**
   - 전역 키보드 후킹
   - 샌드박스에서 차단됨

2. **GetAsyncKeyState 폴링**
   - 직접 키 상태 확인
   - 일반 프로그램 ✅
   - 샌드박스 브라우저 ❌

3. **복합 방식 (Hook + Polling)**
   - 두 가지 동시 시도
   - 브라우저에서는 여전히 실패

### 향후 연구 방향
**커널 드라이버 기반 키로거:**
- Ring 0 레벨에서 키보드 인터럽트 직접 후킹
- 모든 샌드박스 우회 가능
- Windows Driver Kit (WDK) 필요
- 디지털 서명 필요 또는 테스트 모드

## 최종 결과

### 성공한 것 
- 프로세스 자동 감지 및 모니터링
- 자동 DLL 인젝션
- 폴링 방식 키로거 개발
- 일반 Win32 애플리케이션 키로깅

### 확인한 한계 
- 브라우저 샌드박스의 강력한 보호
- 사용자 모드 후킹의 제약
- API 필터링 메커니즘

### 학습 성과
- 실제 보안 메커니즘 경험
- 다양한 우회 기법 시도
- 커널 레벨 필요성 이해
- 현대 OS 보안 아키텍처 학습

## 보안 고려사항

**교육적 가치:**
이 프로젝트를 통해 다음을 학습했습니다:
- 악성코드가 어떻게 작동하는지
- 현대 OS가 어떻게 방어하는지
- 왜 샌드박스가 중요한지
- 커널 레벨 보호의 필요성

**실제 방어 방법:**
- 샌드박스 활용 (Chrome, Edge, Adobe)
- 낮은 무결성 레벨 프로세스
- API 필터링
- 커널 패치 가드
- 디지털 서명 강제
