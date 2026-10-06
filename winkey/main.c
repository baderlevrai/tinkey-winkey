#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <winsvc.h>

void WINAPI ServiceCtrlHandler(DWORD control) {

}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv) {
    SERVICE_STATUS_HANDLE   statusHandle;

    statusHandle = RegisterServiceCtrlHandler(TEXT("NameService"), ServiceCtrlHandler);
}

int main(int ac, char **av) {
    if (ac > 1) { // launched by user

    } else { // launched by SCM
        SERVICE_TABLE_ENTRY serviceTable[] =
        {
            {"NameService", ServiceMain},
            {NULL, NULL}
        };
        StartServiceCtrlDispatcher(serviceTable);
    }
}