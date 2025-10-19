# Week 3: 소프트 후킹 프로젝트

## 📋 프로젝트 개요

소프트 후킹(Soft Hooking) 기법을 활용한 교육용 키로거 및 API 후킹 시연 프로그램

**개발 목적**: DLL 인젝션 및 프로세스 인젝션의 기초가 되는 후킹 기법 학습

**최종 목표**: PDF 파일 실행 시 키로거를 자동 실행하는 악성코드 방어/분석 기술 습득

---

## 🎯 개발 프로그램

### 1. keylogger.exe - 교육용 키로거

**기능**:
- 시스템 전역 키보드 입력 캡처
- 실시간 로그 파일 저장
- 타임스탬프 자동 기록

**후킹 기법**:
- `SetWindowsHookEx(WH_KEYBOARD_LL)` 사용
- Low-Level 키보드 후킹
- 메시지 루프 기반 이벤트 처리

**동작 원리**:
1. 시스템에 키보드 후크 설치
2. 모든 키 입력 이벤트를 콜백 함수로 전달
3. 키 코드를 문자로 변환하여 로그 파일에 기록
4. Shift, CapsLock 상태 확인하여 대소문자 처리

**기술적 특징**:
- 프로세스 경계를 넘는 시스템 전역 후킹
- 특수 키(Backspace, Enter 등) 감지 및 로깅
- 실시간 파일 I/O로 데이터 손실 방지

### 2. hook_demo.exe - API 후킹 데모

**기능**:
- MessageBoxA API 함수 후킹
- 메시지 내용 자동 변조
- IAT 후킹 기법 시연

**후킹 기법**:
- IAT(Import Address Table) 후킹
- PE 헤더 파싱 및 수정
- 메모리 보호 속성 변경

**동작 원리**:
1. 프로세스의 IAT에서 MessageBoxA 주소 탐색
2. 메모리 보호 속성을 쓰기 가능으로 변경
3. 원본 함수 주소를 후킹 함수 주소로 교체
4. 모든 MessageBoxA 호출이 후킹 함수로 리다이렉트

**시연 효과**:
- 원본: "This is a normal message"
- 후킹 후: "This message was intercepted and modified by hook!"

---

## 🔬 소프트 후킹 기술 설명

### 후킹(Hooking)이란?

프로그램의 정상적인 실행 흐름을 가로채어 임의의 코드를 실행하는 기법

**종류**:
- **하드웨어 후킹**: CPU 디버그 레지스터 사용
- **소프트 후킹**: 코드나 데이터 구조 수정

### 소프트 후킹 기법 분류

#### 1. API 후킹
- IAT 후킹: Import Table 수정
- Inline 후킹: 함수 시작 부분 코드 패치
- EAT 후킹: Export Table 수정

#### 2. 프로세스 기반 후킹
- SetWindowsHookEx: 시스템 후크
- DLL Injection: DLL을 타겟 프로세스에 주입
- Code Injection: 쉘코드 직접 주입

### 본 프로젝트에서 사용한 기법

**keylogger.exe**: 
- Windows Hook API 사용
- WH_KEYBOARD_LL: Low-Level 키보드 후킹
- 시스템 전역 범위 후킹

**hook_demo.exe**:
- IAT 후킹 기법
- PE 포맷 구조 활용
- 런타임 메모리 패치

---

## 💻 실행 방법

### 환경 요구사항
- Windows 7 이상
- 관리자 권한 (키로거의 경우)

### 키로거 테스트

1. `keylogger.exe` 실행
2. 메모장이나 브라우저에서 타이핑
3. `logs/keylog.txt` 파일 확인
4. Ctrl+C로 종료

**로그 파일 예시**:
```
[Sun Oct 19 15:35:50 2025] Keylogger started
Hello World[ENTER]
Password 1234[ENTER]
```

### API 후킹 데모 테스트

1. `hook_demo.exe` 실행
2. Enter 키를 눌러 MessageBox 호출
3. 메시지 내용이 자동으로 변조되는 것 확인

**예상 결과**:
- 콘솔: 후킹 과정 로그 출력
- MessageBox: 변조된 메시지 표시

---
