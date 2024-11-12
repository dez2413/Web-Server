// test.c
// Name: Desiree Gutierrez and Tyreke
//Class: CS484 Computer Networking
// Date: 11/6/2024

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Server.h"
// #include "index.html"

int main(){
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 10, launch);
    server.launch(&server);

    return 0;
}