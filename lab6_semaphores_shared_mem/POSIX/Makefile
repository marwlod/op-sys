CFLAGS=-Wall -std=gnu99 -lpthread -lrt

all: barber client_spawner

barber: barber.c
	gcc barber.c -o barber $(CFLAGS)

client_spawner: client_spawner.c
	gcc client_spawner.c -o client_spawner $(CFLAGS)
clean:
	 rm barber client_spawner