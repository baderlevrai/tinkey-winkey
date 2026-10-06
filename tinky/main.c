#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <winsvc.h>

LRESULT CALLBACK foo(int ncode, WPARAM wparam, LPARAM lparam)
{
	int vkCode = ((KBDLLHOOKSTRUCT *)lparam)->vkCode;

	if (wparam == 257) // when released
	{
		if (vkCode >= 65 && vkCode <= 90)
			printf("Pressed: %c\n", vkCode);
		else
			printf("Pressed: <%u>\n");
	}
	return (CallNextHookEx(NULL, ncode, wparam, lparam));
}

int main(void)
{
	HHOOK hook = SetWindowsHookEx(
		WH_KEYBOARD_LL, // Type de hook
		foo,			// Fonction a appeler
		NULL,			// Ou trouver la fonction (null car osef)
		0);				// Thread ID
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg); // jsp ca fais quoi sah
	}
	printf("haha\n");
}