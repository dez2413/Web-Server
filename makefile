all: test

test: Server.h Server.c 
	gcc Server.c -o test

clean:
	rm -f lab4