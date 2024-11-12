// Server.c 
// Name: Desiree Gutierrez and Tyreke Hetzel
// Class: CS484 Computer Networking
// Date: 11/6/2024

#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define Port 8080

struct Server server_constructor(int domain, int service, int protocol, long interface, int backlog){
    struct Server server;

    //config a the struct server
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.backlog = backlog;

    server.address.sin_family = domain;
    server.address.sin_port = htons(Port);
    server.address.sin_addr.s_addr = interface;
    server.addrlen = sizeof(server.address);

    //Test of socket and create a socket
    server.socket = socket(domain, service, protocol);
    if(server.socket < 0) {
        perror("Failed to connect socket...\n");
        exit(1);
    }
    printf("Socket created successfully.\n");

    if ((bind(server.socket, (struct sockaddr*) &server.address, sizeof(server.address))) < 0){
            perror("failed to bind socket... \n");
            exit(1);
        }
        printf("Socket bound to port %d.\n", Port);


    if ((listen(server.socket, server.backlog)) <0){
        perror("failed to bind listen... \n");
        exit(1);
    }
    printf("Listening for incoming connections...\n");

    return server;
   
}


int main(){
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 10);
    
 while (1){
        printf("======== WAITING FOR CONNECTION ========\n");
         server.addrlen = sizeof(server.address);
        int new_socket = accept(server.socket, (struct sockaddr*) &server.address, (socklen_t *) &server.addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Accepted a new connection.\n");
        
        char *hello = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
        socklen_t address_length = sizeof(server.address);
        
        send(new_socket, hello, strlen(hello), 0);
        close(new_socket);
    }
    close(server.socket);
    return 0;
}


