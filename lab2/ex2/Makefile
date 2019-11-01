CC = gcc
CFlags = -Wall

all: search_nftw search_stat

search_nftw: search_nftw.c
	$(CC) $(CFlags) search_nftw.c -o search_nftw

search_stat: search_stat.c
	$(CC) $(CFlags) search_stat.c -o search_stat

clean:
	rm *.o search_nftw search_stat