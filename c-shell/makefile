CC = gcc
CFLAGS = -std=c23 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -Wall -Wextra -Werror -Wno-unused-parameter -fno-asm -I./include

SRCS = src/main.c src/prompt.c src/lexer.c

all: shell.out

shell.out: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o shell.out

clean:
	rm -f shell.out