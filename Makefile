CC=gcc
CFLAGS=-Wall -Wextra

OBJS = server.o

all: server.exe

server.exe: $(OBJS)
	gcc -o $@ $^

server.o: server.c

clean:
	rm -f $(OBJS)
	rm -r server.exe