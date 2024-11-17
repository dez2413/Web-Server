// Server.h
// Names: Desiree Gutierrez and Tyreke Hetzel
// Class: CS484 Computer Networking
// Date: 11/6/2024
// Purpose: This header file defines the structure and function prototypes for the Server used in a multi-threaded HTTP server. 
// It includes the `Server` struct definition and the prototype for the server constructor function, which initializes the server with specified parameters for domain, service, protocol, interface, and backlog.

#ifndef Server_h
#define Server_h

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>

// Structure to represent the Server
struct Server {
    int domain;               // Address family (AF_INET for IPv4)
    int service;              // Socket type (SOCK_STREAM for TCP)
    int protocol;             // Protocol to be used (IPPROTO_TCP)
    uint32_t interface;       // Network interface (INADDR_ANY for all available interfaces)
    int backlog;              // Maximum number of pending connections in the queue

    struct sockaddr_in address; // Server address structure
    int addrlen;                // Length of the address structure
    int socket;                 // Socket file descriptor

    // Function pointer for launching the server (not used in the current implementation)
    void (*launch)(struct Server *server);
};

// Constructs a server with the specified domain, service, protocol, interface, and backlog.
struct Server server_constructor(int domain, int service, int protocol, uint32_t interface, int backlog);

#endif /* end of Server_h */
