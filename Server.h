// Server.h 
// Name: Desiree Gutierrez
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
    int port;
    int backlog;

    struct sockaddr_in address;

    int socket;

    void (*launch) (struct Server *server);
};

void launch(struct Server *server);

struct Server server_constructor(int domain, int service, int protocol,
 long interface, int port, int backlog, void (*launch) (struct Server *server));

#endif /*end of server.h*/
