all: test

test: Server.h Server.c test.c
	gcc Server.c test.c -o test

clean:
	rm -f lab4