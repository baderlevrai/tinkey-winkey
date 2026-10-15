NAME    = winkey.exe
SRCS    = main.c
OBJS    = $(SRCS:.c=.o)

CC      = gcc
CFLAGS  = -Wall -Wextra
LDLIBS  = -luser32

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /q $(OBJS)

fclean: clean
	del /q $(NAME)

re: fclean all

run: all
	$(NAME)

.PHONY: all clean fclean re run
