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
	rm *_test *_test_o* fib mallocs insertion string_to_list

test: test_fib test_insertion test_mallocs test_string_to_list

test_fib:
	$(CC) $(CFlags) fib.c -o fib_test
	./fib_test
	$(CC) $(CFlags) fib.c -o fib_test_o1 -O1
	./fib_test_o1
	$(CC) $(CFlags) fib.c -o fib_test_o2 -O2
	./fib_test_o2
	$(CC) $(CFlags) fib.c -o fib_test_o3 -O3
	./fib_test_o3

test_mallocs:
	$(CC) $(CFlags) mallocs.c -o mallocs_test
	./mallocs_test
	$(CC) $(CFlags) mallocs.c -o mallocs_test_o1 -O1
	./mallocs_test_o1
	$(CC) $(CFlags) mallocs.c -o mallocs_test_o2 -O2
	./mallocs_test_o2
	$(CC) $(CFlags) mallocs.c -o mallocs_test_o3 -O3
	./mallocs_test_o3

test_insertion:
	$(CC) $(CFlags) insertion.c -o insertion_test
	./insertion_test
	$(CC) $(CFlags) insertion.c -o insertion_test_o1 -O1
	./insertion_test_o1
	$(CC) $(CFlags) insertion.c -o insertion_test_o2 -O2
	./insertion_test_o2
	$(CC) $(CFlags) insertion.c -o insertion_test_o3 -O3
	./insertion_test_o3

test_string_to_list:
	$(CC) $(CFlags) string_to_list.c -o string_to_list_test
	./string_to_list_test
	$(CC) $(CFlags) string_to_list.c -o string_to_list_test_o1 -O1
	./string_to_list_test_o1
	$(CC) $(CFlags) string_to_list.c -o string_to_list_test_o2 -O2
	./string_to_list_test_o2
	$(CC) $(CFlags) string_to_list.c -o string_to_list_test_o3 -O3
	./string_to_list_test_o3
