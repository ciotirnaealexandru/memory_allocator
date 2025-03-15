CC=gcc
CFLAGS=-Wall -Wextra -c
EXE=allocator

build: allocator.o
	$(CC) $^ -o $(EXE)

alcator.o: allocator.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf allocator.o $(EXE)


