NAME = tinky.exe
OBJS = main.obj

CC = cl
CFLAGS = /Wall /WX
LDFLAGS = /link user32.lib

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) /Fe:$(NAME) $(LDFLAGS)

.c.obj:
	$(CC) $(CFLAGS) /c $<

clean:
	del /q $(OBJS)

fclean: clean
	del /q $(NAME)

re: fclean all

run: all
	$(NAME)
