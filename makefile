all: test

test: Server.h Server.c queue.c queue.h
	gcc Server.c queue.c -o test

clean:
	rm -f test

server: test
	./test
