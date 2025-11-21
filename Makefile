CC = gcc
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined
LDFLAGS = -fsanitize=address,undefined

all: dvcp

dvcp: dvcp.c
	$(CC) $(CFLAGS) -o dvcp dvcp.c $(LDFLAGS)

clean:
	rm -f dvcp *.o

test: dvcp
	@echo "IMG" > test_input.txt
	./dvcp test_input.txt

.PHONY: all clean test
