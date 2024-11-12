// Server.c 
// Name: Desiree Gutierrez
//Class: CS484 Computer Networking
// Date: 11/6/2024

#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define Port 8080

struct Server server_constructor(int domain, int service, int protocol, long interface, int backlog, void (*launch)(struct Server *server)){
    struct Server server;

    //config a the struct server
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.backlog = backlog;

    server.addrlen = sizeof(server.address);
    server.address.sin_family = domain;
    server.address.sin_port = htons(Port);
    server.address.sin_addr.s_addr = interface;

    //Test of socket and create a socket
    server.socket = socket(domain, service, protocol);
    printf("1\n");
    if(server.socket < 0) {
        perror("Failed to connect socket...\n");
        exit(1);
    }
    printf("Socket created successfully.\n");

    if ((bind(server.socket, (struct sockaddr*) &server.address,
        sizeof(server.address))) <0){
            perror("failed to bind socket... \n");
            exit(1);
        }
        printf("Socket bound to port %d.\n", Port);


    if ((listen(server.socket, server.backlog)) <0){
        perror("failed to bind listen... \n");
        exit(1);
    }
    printf("Listening for incoming connections...\n");

    server.launch = launch;
    return server;
}

void launch(struct Server *server){

    char buffer[30000];
    char *hello = "HTTP/1.1 200 OK\r\nContent-Type: text/html\nConnection: closed\n <html><body><h1>Hello, World!</h1></body></html>";
    int address_length = sizeof(server->address);
    int new_socket = accept(server->socket, (struct sockaddr *) &server->address, (socklen_t *) &address_length);
    while (1){
    
        printf("======== WAITING FOR CONNECTION ========\n");
        read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        write (new_socket, hello, strlen(hello));
        close(new_socket);
    }
}

