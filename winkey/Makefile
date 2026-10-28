NAME    = winkey.exe
SRCS    = main.c
OBJS    = $(SRCS:.c=.obj)

CC      = cl
CFLAGS  = /Wall /WX /wd5045 /nologo
LDLIBS  = user32.lib

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) /Fe$(NAME) /link $(LDLIBS)

.c.obj:
	$(CC) $(CFLAGS) /c $< /Fo$@

clean:
	del /q $(OBJS) 2>nul

fclean: clean
	del /q $(NAME) 2>nul

re: fclean all

run: all
	$(NAME)
