CC=gcc
CFLAGS=-Wall -Wextra -g -O0

OBJS = server.o

all: server.exe

server.exe: $(OBJS)
	gcc -o $@ $^

server.o: server.c

clean:
	rm -f $(OBJS)
	rm -r server.exe