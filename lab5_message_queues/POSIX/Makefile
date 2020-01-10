CFLAGS=-Wall -std=gnu99 -static -Wall -Wextra -Isrc -rdynamic -lrt

all: client server

client: client.c
	gcc client.c -o client $(CFLAGS)

server: server.c
	gcc server.c -o server $(CFLAGS)

clean:
	 rm client server