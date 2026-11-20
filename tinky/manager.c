#include "tinky.h"

void    print_err(char *str) {
    fprintf(stderr, "Fatal Error: %s: %lu", str, GetLastError());
}

int sc_install(void) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!scm)
        return (print_err("OpenSCManager"), 1);
    char path[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, path, MAX_PATH);
    SC_HANDLE svc = CreateService(
        scm,
        SC_NAME,
        SC_DISPLAY_NAME,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        path,
        NULL, NULL, NULL, NULL, NULL
    );
    CloseServiceHandle(scm);
    if (!svc)
        return (print_err("CreateService"), 1);
    CloseServiceHandle(svc);
    printf("Service Installed successfully ! =)\n");
    return (0);
}

int sc_start(void) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm)
        return (print_err("OpenSCManager"), 1);
    SC_HANDLE svc = OpenService(scm, SC_NAME, SERVICE_START | SERVICE_QUERY_STATUS);
    if (!svc)
        return (CloseServiceHandle(scm), print_err("OpenService"), 1);
    if (!StartService(svc, 0, NULL)) {
        CloseServiceHandle(scm);
        CloseServiceHandle(svc);
        print_err("StartService");
        return (1);
    }
    CloseServiceHandle(scm);
    CloseServiceHandle(svc);
    return (0);
}

int sc_stop(void) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm)
        return (print_err("OpenSCManager"), 1);
    SC_HANDLE svc = OpenService(scm, SC_NAME, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!svc)
        return (CloseServiceHandle(scm), print_err("OpenService"), 1);
    SERVICE_STATUS status;
    if (!ControlService(svc, SERVICE_CONTROL_STOP, &status)) {
        CloseServiceHandle(scm);
        CloseServiceHandle(svc);
        print_err("ControlService");
        return (1);
    }

    while (status.dwCurrentState != SERVICE_STOPPED) {
        Sleep(1000);
        if (!QueryServiceStatus(svc, &status))
            break;
    }
    printf("Serivce stopped =(\n");
    CloseServiceHandle(scm);
    CloseServiceHandle(svc);
    return (0);
}

int sc_delete(void) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm)
        return (print_err("OpenSCManager"), 1);
    SC_HANDLE svc = OpenService(scm, SC_NAME, DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!svc)
        return (CloseServiceHandle(scm), print_err("OpenService"), 1);

    //stopping
    SERVICE_STATUS status = { 0 };
    if (!QueryServiceStatus(svc, &status)) {
        CloseServiceHandle(scm);
        CloseServiceHandle(svc);
        print_err("QueryServiceStatus");
        return (1);
    }
    if (status.dwCurrentState != SERVICE_STOPPED) {
        if (!ControlService(svc, SERVICE_CONTROL_STOP, &status)) {
            CloseServiceHandle(scm);
            CloseServiceHandle(svc);
            print_err("ControlService");
            return (1);
        }

        int timeout = 0;
        while (status.dwCurrentState != SERVICE_STOPPED && timeout != 50) {
            Sleep(1000);
            timeout++;
            if (!QueryServiceStatus(svc, &status))
                break;
        }
        if (timeout >= 50) {
            CloseServiceHandle(scm);
            CloseServiceHandle(svc);
            fprintf(stderr, "Fatal error: Timeout");
        }
        printf("Serivce stopped =(\n");
    }

    if (!DeleteService(svc)) {
        CloseServiceHandle(scm);
        CloseServiceHandle(svc);
        if (GetLastError() == ERROR_SERVICE_MARKED_FOR_DELETE)
            printf("Service already deleted ='(");
        else
            print_err("DeleteService");
        return (1);
    }
    printf("Service successfuly deleted ='(\n");
    CloseServiceHandle(scm);
    CloseServiceHandle(svc);
    return (0);
}

int manager_dispatch(char **arg) {
    if (!strcmp(arg[1], "install"))
        return (sc_install());
    if (!strcmp(arg[1], "start"))
        return (sc_start());
    if (!strcmp(arg[1], "stop"))
        return (sc_stop());
    if (!strcmp(arg[1], "delete"))
        return (sc_delete());
    fprintf(stderr, "Usage: %s [install | start | stop | delete]\n", arg[0]);
    return (1);
}