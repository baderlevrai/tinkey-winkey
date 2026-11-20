NMAKE_EXE   = nmake
NMAKE_FLAGS = /nologo

!IFNDEF USER
USER = bader
!ENDIF

all:
	@cd tinky && $(NMAKE_EXE) $(NMAKE_FLAGS) all
	@cd winkey && $(NMAKE_EXE) $(NMAKE_FLAGS) all

clean:
	-@cd tinky && $(NMAKE_EXE) $(NMAKE_FLAGS) clean
	-@cd winkey && $(NMAKE_EXE) $(NMAKE_FLAGS) clean

fclean:
	-@cd tinky && $(NMAKE_EXE) $(NMAKE_FLAGS) fclean
	-@cd winkey && $(NMAKE_EXE) $(NMAKE_FLAGS) fclean

re: fclean all

install: all
	move .\winkey\winkey.exe C:\Users\$(USER)\\
	tinky\svc.exe install
	tinky\svc.exe start