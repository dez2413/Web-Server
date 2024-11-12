// Server.h 
// Name: Desiree Gutierrez and Tyreke
//Class: CS484 Computer Networking
// Date: 11/6/2024

#ifndef Server_h
#define Server_h

#include <sys/socket.h>
#include <netinet/in.h>

struct Server{
    int domain;
    int service;
    int protocol;
    long interface;
    int backlog;

    struct sockaddr_in address;
    int addrlen;
    int socket;

    void (*launch) (struct Server *server);
};


struct Server server_constructor(int domain, int service, int protocol,
 long interface, int backlog);

#endif /*end of server.h*/
