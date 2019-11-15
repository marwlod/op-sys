CC = gcc
CFlags = -Wall

all: program

program: program.c
	$(CC) $(CFlags) program.c -o program

clean:
	rm *.o dane1 dane2 dane3 program

test: test_sort_size_4 test_copy_size_4 test_sort_size_512 test_copy_size_512
	test_sort_size_4096 test_copy_size_4096 test_sort_size_8192 test_copy_size_8192

test_sort_size_4:
	./program generate dane1 2000 4
	cp dane1 dane2
	./program sort dane1 2000 4 sys
	./program sort dane2 2000 4 lib

	./program generate dane1 5000 4
	cp dane1 dane2
	./program sort dane1 5000 4 sys
	./program sort dane2 5000 4 lib


test_copy_size_4:
	./program generate dane1 2000000 4
	./program copy dane1 dane3 2000000 4 sys
	./program copy dane1 dane3 2000000 4 lib

	./program generate dane1 5000000 4
	./program copy dane1 dane3 5000000 4 sys
	./program copy dane1 dane3 5000000 4 lib

test_sort_size_512:
	./program generate dane1 2000 512
	cp dane1 dane2
	./program sort dane1 2000 512 sys
	./program sort dane2 2000 512 lib

	./program generate dane1 5000 512
	cp dane1 dane2
	./program sort dane1 5000 512 sys
	./program sort dane2 5000 512 lib

test_copy_size_512:
	./program generate dane1 2000000 512
	./program copy dane1 dane3 2000000 512 sys
	./program copy dane1 dane3 2000000 512 lib

	./program generate dane1 5000000 512
	./program copy dane1 dane3 5000000 512 sys
	./program copy dane1 dane3 5000000 512 lib

test_sort_size_4096:
	./program generate dane1 1000 4096
	cp dane1 dane2
	./program sort dane1 1000 4096 sys
	./program sort dane2 1000 4096 lib

	./program generate dane1 2500 4096
	cp dane1 dane2
	./program sort dane1 2500 4096 sys
	./program sort dane2 2500 4096 lib

test_copy_size_4096:
	./program generate dane1 2000000 4096
	./program copy dane1 dane3 2000000 4096 sys
	./program copy dane1 dane3 2000000 4096 lib

	./program generate dane1 5000000 4096
	./program copy dane1 dane3 5000000 4096 sys
	./program copy dane1 dane3 5000000 4096 lib

test_sort_size_8192:
	./program generate dane1 1000 8192
	cp dane1 dane2
	./program sort dane1 1000 8192 sys
	./program sort dane2 1000 8192 lib

	./program generate dane1 2500 8192
	cp dane1 dane2
	./program sort dane1 2500 8192 sys
	./program sort dane2 2500 8192 lib

test_copy_size_8192:
	./program generate dane1 1000000 8192
	./program copy dane1 dane3 1000000 8192 sys
	./program copy dane1 dane3 1000000 8192 lib

	./program generate dane1 2500000 8192
	./program copy dane1 dane3 2500000 8192 sys
	./program copy dane1 dane3 2500000 8192 lib