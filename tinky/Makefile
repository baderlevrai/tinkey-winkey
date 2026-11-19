NAME    = svc.exe
SRCS    = main.c manager.c service.c
OBJS    = $(SRCS:.c=.obj)

CC      = cl
CFLAGS  = /Wall /WX /nologo
LDLIBS  = advapi32.lib

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) /Fe$(NAME) /link $(LDLIBS)

.c.obj:
	$(CC) $(CFLAGS) /c $< /Fo$@

clean:
	del /q $(OBJS)

fclean: clean
	del /q $(NAME)

re: fclean all

run: all
	$(NAME)
