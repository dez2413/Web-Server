all: server

server: Server.h Server.c 
	gcc Server.c -o server

clean:
	rm -f lab4

run: server
	./server