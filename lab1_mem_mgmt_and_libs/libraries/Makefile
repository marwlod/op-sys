CC = gcc
CFlags = -Wall

all: static_check shared_check dynamic_check

static_check: static_check.c
	$(CC) $(CFlags) -c sort_library.c
	ar rcs libsort_library.a sort_library.o
	$(CC) $(CFlags) -c static_check.c
	$(CC) $(CFlags) static_check.o libsort_library.a -o static_check

shared_check: shared_check.c
	$(CC) $(CFlags) -c -fPIC sort_library.c
	$(CC) $(CFlags) -shared -fPIC -o libsort_library.so sort_library.o
	$(CC) $(CFlags) -o shared_check shared_check.c -L. libsort_library.so -I .

dynamic_check: dynamic_check.c
	$(CC) $(CFlags) dynamic_check.c -o dynamic_check -Wl,--no-as-needed -ldl

clean:
	rm *.o *_test *_test_o* static_check shared_check dynamic_check *.so *.a

test: test_static test_dynamic test_shared

test_static:
	$(CC) $(CFlags) static_check.c -o static_check_test
	$(CC) $(CFlags) static_check.c -o static_check_test_o1 -O1
	$(CC) $(CFlags) static_check.c -o static_check_test_o2 -O2
	$(CC) $(CFlags) static_check.c -o static_check_test_o3 -O3

test_dynamic:
	$(CC) $(CFlags) dynamic_check.c -o dynamic_check_test -Wl,--no-as-needed -ldl
	$(CC) $(CFlags) dynamic_check.c -o dynamic_check_test_o1 -O1 -Wl,--no-as-needed -ldl
	$(CC) $(CFlags) dynamic_check.c -o dynamic_check_test_o2 -O2 -Wl,--no-as-needed -ldl
	$(CC) $(CFlags) dynamic_check.c -o dynamic_check_test_o3 -O3 -Wl,--no-as-needed -ldl

test_shared:
	make shared_check
	$(CC) $(CFlags) shared_check.c -o shared_check_test -L. libsort_library.so -I .
	$(CC) $(CFlags) shared_check.c -o shared_check_test_o1 -O1 -L. libsort_library.so -I .
	$(CC) $(CFlags) shared_check.c -o shared_check_test_o2 -O2 -L. libsort_library.so -I .
	$(CC) $(CFlags) shared_check.c -o shared_check_test_o3 -O3 -L. libsort_library.so -I .

