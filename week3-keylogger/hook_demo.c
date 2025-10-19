#include <windows.h>
#include <stdio.h>

// MessageBoxA 함수 포인터 타입 정의
typedef int (WINAPI *MessageBoxA_t)(HWND, LPCSTR, LPCSTR, UINT);

// 원본 MessageBoxA 함수 포인터
MessageBoxA_t originalMessageBoxA = NULL;

// 후킹된 MessageBoxA 함수
int WINAPI HookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    printf("\n[HOOK] MessageBoxA called!\n");
    printf("[HOOK] Original Title: %s\n", lpCaption);
    printf("[HOOK] Original Text: %s\n", lpText);
    
    // 메시지 내용 변경
    const char* newCaption = "[HOOKED] Message Intercepted!";
    const char* newText = "This message was intercepted and modified by hook!";
    
    printf("[HOOK] Modifying message...\n\n");
    
    // 수정된 내용으로 원본 함수 호출
    return originalMessageBoxA(hWnd, newText, newCaption, uType);
}

// IAT(Import Address Table) 후킹
BOOL HookIAT(LPCSTR moduleName, LPCSTR functionName, LPVOID hookFunction, LPVOID *originalFunction) {
    // 현재 프로세스의 베이스 주소
    HMODULE hModule = GetModuleHandle(NULL);
    if (!hModule) return FALSE;
    
    // PE 헤더 파싱
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)(
        (BYTE*)hModule + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
    );
    
    // IAT에서 함수 찾기
    while (importDesc->Name) {
        char* dllName = (char*)((BYTE*)hModule + importDesc->Name);
        
        if (_stricmp(dllName, moduleName) == 0) {
            PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + importDesc->FirstThunk);
            PIMAGE_THUNK_DATA origThunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + importDesc->OriginalFirstThunk);
            
            while (thunk->u1.Function) {
                if (origThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
                    thunk++;
                    origThunk++;
                    continue;
                }
                
                PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)(
                    (BYTE*)hModule + origThunk->u1.AddressOfData
                );
                
                if (strcmp((char*)importByName->Name, functionName) == 0) {
                    // 메모리 보호 속성 변경
                    DWORD oldProtect;
                    VirtualProtect(&thunk->u1.Function, sizeof(LPVOID), PAGE_READWRITE, &oldProtect);
                    
                    // 원본 함수 포인터 저장
                    *originalFunction = (LPVOID)thunk->u1.Function;
                    
                    // 후킹 함수로 교체
                    thunk->u1.Function = (DWORD_PTR)hookFunction;
                    
                    // 메모리 보호 속성 복원
                    VirtualProtect(&thunk->u1.Function, sizeof(LPVOID), oldProtect, &oldProtect);
                    
                    return TRUE;
                }
                
                thunk++;
                origThunk++;
            }
        }
        
        importDesc++;
    }
    
    return FALSE;
}

int main() {
    printf("=================================\n");
    printf("API Hooking Demo\n");
    printf("=================================\n\n");
    
    // MessageBoxA 후킹
    printf("[*] Hooking MessageBoxA...\n");
    if (HookIAT("user32.dll", "MessageBoxA", HookedMessageBoxA, (LPVOID*)&originalMessageBoxA)) {
        printf("[+] Hook installed successfully!\n\n");
    } else {
        printf("[-] Failed to install hook!\n");
        return 1;
    }
    
    printf("Press Enter to test normal MessageBox...\n");
    getchar();
    
    // 후킹 전 테스트
    printf("\n[TEST 1] Calling MessageBoxA...\n");
    MessageBoxA(NULL, "This is a normal message", "Normal Message", MB_OK);
    
    printf("\nPress Enter to continue...\n");
    getchar();
    
    printf("\n[TEST 2] Calling MessageBoxA again...\n");
    MessageBoxA(NULL, "This message will also be hooked", "Another Message", MB_OK | MB_ICONINFORMATION);
    
    printf("\n=================================\n");
    printf("Demo completed!\n");
    printf("=================================\n");
    
    return 0;
}
