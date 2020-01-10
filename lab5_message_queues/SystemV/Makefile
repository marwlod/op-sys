CFLAGS=-Wall -std=gnu99 -static -Wall -Wextra -Isrc -rdynamic -lrt

all: client server

client: client.c
	gcc $(CFLAGS) client.c -o client

server: server.c
	gcc $(CFLAGS) server.c -o server

clean:
	 rm client server