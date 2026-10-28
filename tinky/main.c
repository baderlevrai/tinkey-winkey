#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <winsvc.h>
#include <string.h>
#include <TlHelp32.h>

SERVICE_STATUS          g_status;
HANDLE                  g_stopEvent;
SERVICE_STATUS_HANDLE   g_statusHandle;

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

}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv) {
    g_statusHandle = RegisterServiceCtrlHandler(TEXT("tinky"), ServiceCtrlHandler);
    g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_status.dwCurrentState = SERVICE_START_PENDING;
    g_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    SetServiceStatus(g_statusHandle, &g_status);

    //GET TOKEN WINLOGON

    HANDLE hsys = get_process_handle("winlogon.exe");
    if (!hsys) {
        // belek
        return ;
    }

    HANDLE htoken = NULL;
    if (!OpenProcessToken(hsys, TOKEN_ALL_ACCESS, &htoken)) {
        // belek
        return ;
    }

    HANDLE hduptoken = NULL;
    if (!DuplicateTokenEx(htoken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hduptoken)) {
        //belek
        return ;
    }

    // LAUNCH PROCESS

    STARTUPINFOA        si = {0};
    PROCESS_INFORMATION pi = {0};

    si.cb = sizeof(si);

    if (!CreateProcessAsUser(hduptoken, NULL, ".\\winkey.exe", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, argv[1], &si, &pi)) {
        return ;
    }

    // CreateProcess(NULL, ".\\winkey.exe", NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
    g_status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_statusHandle, &g_status);
    WaitForSingleObject(g_stopEvent, INFINITE);
    g_status.dwCurrentState = SERVICE_STOP_PENDING;
    SetServiceStatus(g_statusHandle, &g_status);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(htoken);
    CloseHandle(hduptoken);
    g_status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_statusHandle, &g_status);
}

int main(int ac, char **av) {
    if (ac > 1) { // launched by user
        //av[1] == {"install","start","stop","delete"}
    } else { // launched by SCM
        SERVICE_TABLE_ENTRY serviceTable[] =
        {
            {"tinky", ServiceMain},
            {NULL, NULL}
        };
        StartServiceCtrlDispatcher(serviceTable);
    }
}
