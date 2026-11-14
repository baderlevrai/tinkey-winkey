#include "tinky.h"

int sc_install(void) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!scm)
        return (fprintf(stderr, "fatal error"), 1);
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
        return (fprintf(stderr, "fatal error"), 1);
    CloseServiceHandle(svc);
    printf("Service Installed successfully ! =)\n");
    return (0);
}

int sc_start(void) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm)
        return (fprintf(stderr, "fatal error"), 1);
    SC_HANDLE svc = OpenService(scm, SC_NAME, SERVICE_START | SERVICE_QUERY_STATUS);
    if (!svc)
        return (CloseServiceHandle(scm), fprintf(stderr, "fatal error"), 1);
    if (!StartService(svc, 0, NULL)) {
        CloseServiceHandle(scm);
        CloseServiceHandle(svc);
        fprintf(stderr, "fatal error");
        return (1);
    }
    CloseServiceHandle(scm);
    CloseServiceHandle(svc);
    return (0);
}

int sc_stop(void) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm)
        return (fprintf(stderr, "fatal error"), 1);
    SC_HANDLE svc = OpenService(scm, SC_NAME, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!svc)
        return (CloseServiceHandle(scm), fprintf(stderr, "fatal error"), 1);
    SERVICE_STATUS status;
    if (!ControlService(svc, SERVICE_CONTROL_STOP, &status)) {
        CloseServiceHandle(scm);
        CloseServiceHandle(svc);
        fprintf(stderr, "fatal error");
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
        return (fprintf(stderr, "fatal error"), 1);
    SC_HANDLE svc = OpenService(scm, SC_NAME, DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (!svc)
        return (CloseServiceHandle(scm), fprintf(stderr, "fatal error"), 1);

    //stopping
    SERVICE_STATUS status;
    if (!ControlService(svc, SERVICE_CONTROL_STOP, &status)) {
        CloseServiceHandle(scm);
        CloseServiceHandle(svc);
        fprintf(stderr, "fatal error");
        return (1);
    }

    while (status.dwCurrentState != SERVICE_STOPPED) {
        Sleep(1000);
        if (!QueryServiceStatus(svc, &status))
            break;
    }
    printf("Serivce stopped =(\n");

    if (!DeleteService(svc)) {
        CloseServiceHandle(scm);
        CloseServiceHandle(svc);
        fprintf(stderr, "fatal error");
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