# Week 3: 키로거 & API 후킹 프로젝트

## 개요
소프트 후킹 기법을 이용한 교육용 키로거 및 API 후킹 데모

## 프로그램 설명

### 1. keylogger.exe
- **기능**: 키보드 입력을 실시간으로 캡처하여 로그 파일에 저장
- **후킹 방식**: SetWindowsHookEx를 이용한 Low-Level 키보드 후킹
- **로그 위치**: `logs/keylog.txt`

### 2. hook_demo.exe  
- **기능**: MessageBoxA API를 후킹하여 메시지 내용 변조
- **후킹 방식**: IAT(Import Address Table) 후킹
- **시연**: 메시지 박스의 내용이 자동으로 바뀌는 것을 확인

## 실행 방법 (Windows에서)

### 키로거 테스트
1. `keylogger.exe` 더블클릭 실행
2. 메모장이나 다른 프로그램에서 타이핑
3. `logs/keylog.txt` 파일에서 로그 확인
4. Ctrl+C로 종료

### API 후킹 데모
1. `hook_demo.exe` 더블클릭 실행
2. Enter 키를 눌러 MessageBox 호출
3. 메시지 내용이 자동으로 변조되는 것 확인

## 기술적 설명

### 후킹 기법
- **Low-Level 후킹**: 시스템 전역에서 키보드 이벤트 캡처
- **IAT 후킹**: 프로그램의 Import Table을 수정하여 API 함수 가로채기

### 3단계 준비
이 프로젝트는 최종 목표(PDF 실행 시 키로거 실행)의 기반:
- 1단계: 키로거 개발 ✅
- 2단계: DLL 인젝션 (예정)
- 3단계: PDF + 키로거 통합 (예정)


