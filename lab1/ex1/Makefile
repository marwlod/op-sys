CC = gcc
CFlags = -Wall

all: fib mallocs insertion string_to_list

fib: fib.c
	$(CC) $(CFlags) fib.c -o fib

mallocs: mallocs.c
	$(CC) $(CFlags) mallocs.c -o mallocs

insertion: insertion.c
	$(CC) $(CFlags) insertion.c -o insertion

string_to_list: string_to_list.c
	$(CC) $(CFlags) string_to_list.c -o string_to_list

clean:
	rm *.o *_test *_test_o* fib mallocs insertion string_to_list

test:
	$(CC) $(CFlags) fib.c -o fib_test
	$(CC) $(CFlags) fib.c -o fib_test_o1 -O1
	$(CC) $(CFlags) fib.c -o fib_test_o2 -O2
	$(CC) $(CFlags) fib.c -o fib_test_o3 -O3

	$(CC) $(CFlags) mallocs.c -o mallocs_test
	$(CC) $(CFlags) mallocs.c -o mallocs_test_o1 -O1
	$(CC) $(CFlags) mallocs.c -o mallocs_test_o2 -O2
	$(CC) $(CFlags) mallocs.c -o mallocs_test_o3 -O3

	$(CC) $(CFlags) insertion.c -o insertion_test
	$(CC) $(CFlags) insertion.c -o insertion_test_o1 -O1
	$(CC) $(CFlags) insertion.c -o insertion_test_o2 -O2
	$(CC) $(CFlags) insertion.c -o insertion_test_o3 -O3

	$(CC) $(CFlags) string_to_list.c -o string_to_list_test
	$(CC) $(CFlags) string_to_list.c -o string_to_list_test_o1 -O1
	$(CC) $(CFlags) string_to_list.c -o string_to_list_test_o2 -O2
	$(CC) $(CFlags) string_to_list.c -o string_to_list_test_o3 -O3
