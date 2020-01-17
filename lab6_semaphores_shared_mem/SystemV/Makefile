CFLAGS=-Wall -std=gnu99 -static -Wextra -Isrc -rdynamic -lrt

all: barber client_spawner

barber: barber.c
	gcc $(CFLAGS) barber.c -o barber

client_spawner: client_spawner.c
	gcc $(CFLAGS) client_spawner.c -o client_spawner
clean:
	 rm barber client_spawner