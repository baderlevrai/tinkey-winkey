#ifndef TINKY_H
# define TINKY_H

# define WIN32_LEAN_AND_MEAN

# define SC_NAME "tinky"
# define SC_DISPLAY_NAME "Feur Service"

# define WINKY_EXEC_PATH "C:\\Users\\bader\\winkey.exe"
# define WINKY_PATH "C:\\Users\\bader\\"

# pragma warning(push, 0)
# include <stdio.h>
# include <windows.h>
# include <winsvc.h>
# include <string.h>
# include <TlHelp32.h>
# pragma warning(pop)


int         manager_dispatch(char **arg);
void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);

#endif