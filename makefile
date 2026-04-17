CC = gcc
CFLAGS = -Wall -g

all:
	$(CC) $(CFLAGS) src/main.c src/debugger.c -o debugger

clean:
	rm -f debugger
