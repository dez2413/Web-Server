// Server.c
// Names: Desiree Gutierrez and Tyreke Hetzel
// Class: CS484 Computer Networking
// Date: 11/6/2024
// Purpose: This file implements the core functionality of a multi-threaded HTTP server. 
// It includes server setup, handling incoming client connections, sending HTTP responses, and gracefully shutting down the server. 
// The server listens on port 8080 and uses a thread pool to handle multiple clients concurrently.

#include "Server.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define PORT 8080                // Port number for the server to listen on
#define THREAD_POOL_SIZE 1000    // Number of threads in the thread pool
#define BUFFER_SIZE 4096         // Buffer size for handling HTTP requests/responses

// Global variable for the server socket
int server_socket;                  // Server socket file descriptor
ConnectionQueue connection_queue;   // Initialize the connection queue for managing client sockets

// Function to handle SIGINT (Ctrl+C) for graceful shutdown
void handle_sigint(int sig) {
    printf("\nCaught signal %d. Closing socket.\n", sig);
    // Close the server socket to stop accepting new connections
    close(server_socket);
    exit(0); // Exit the program
} // End of handle_sigint

// Function to send HTTP response
void send_response(int client_socket, const char *status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE]; // Buffer to hold the complete HTTP response
    // Format the HTTP response headers and body
    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"           // HTTP status line
             "Content-Type: %s\r\n"      // Content-Type header
             "Content-Length: %ld\r\n"   // Content-Length header
             "Connection: close\r\n"     // Connection header to close the connection after response
             "\r\n"                      // Blank line to indicate end of headers
             "%s",                       // Body of the response
             status, content_type, strlen(body), body);
    // Send the formatted response to the client
    send(client_socket, response, strlen(response), 0);
} // End of send_response

// Function to read the contents of a file
char *read_file(const char *filename) {
    // Open the file in read mode
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file"); // Print an error message if file opening fails
        return NULL;
    }
    // Move the file pointer to the end to determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);   // Get the size of the file in bytes
    fseek(file, 0, SEEK_SET);       // Reset the file pointer to the beginning of the file

    // Allocate a buffer to hold the file contents (+1 for null terminator)
    char *content = malloc(file_size + 1);
    if (content == NULL) {
        perror("Failed to allocate memory"); // Error if memory allocation fails
        fclose(file); // Close the file before returning
        return NULL;
    }
    // Read the entire file into the buffer
    fread(content, 1, file_size, file);
    content[file_size] = '\0'; // Null-terminate the string
    fclose(file); // Close the file after reading its contents
    return content; // Return the content buffer
} // End of read_file

// Function to handle incoming HTTP requests from clients
void handle_request(int client_socket) {
    // Buffer to store the incoming HTTP request data
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    // Receive data from the client and store it in the buffer
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Failed to receive data"); // Error handling for receiving data
        close(client_socket);
        return;
    }
    // Log the received request for debugging purposes
    printf("[INFO] Received request:\n%s\n", buffer);
    // Variables to hold the parsed method and path from the request line
    char method[8], path[1024];
    sscanf(buffer, "%s %s", method, path); // Extract the method and path
    // Handle GET requests
    if (strcmp(method, "GET") == 0) {
        // Handle the root ("/") and "/index.html" paths by serving the index.html file
        if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
            char *html_content = read_file("index.html"); // Read content from index.html
            if (html_content != NULL) {
                send_response(client_socket, "200 OK", "text/html", html_content);
                free(html_content); // Free the allocated memory after sending
            } else {
                send_response(client_socket, "500 Internal Server Error", "text/plain", "Error loading page");
            }
        } 
        // Handle "/google" path by redirecting to Google's website
        else if (strcmp(path, "/google") == 0) {
            const char *redirect_response =
                "HTTP/1.1 301 Moved Permanently\r\n"
                "Location: https://google.com\r\n"
                "Connection: close\r\n\r\n";
            send(client_socket, redirect_response, strlen(redirect_response), 0);
        } 
        // Handle requests for "/favicon.ico" (returning 404 as it is not available)
        else if (strcmp(path, "/favicon.ico") == 0) {
            send_response(client_socket, "404 Not Found", "text/plain", "404 Not Found");
        } 
        // If GET is used on "/multiply", return 405 Method Not Allowed
        else if (strcmp(path, "/multiply") == 0) {
            send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method Not Allowed");
        } 
        // If GET is used on "/database.php", return 405 Method Not Allowed
        else if (strcmp(path, "/database.php") == 0) {
            send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method Not Allowed");
        } 
        // Handle any unrecognized paths with a 404 Not Found response
        else {
            send_response(client_socket, "404 Not Found", "text/plain", "404 Not Found");
        }
    }
    // Handle POST requests
    else if (strcmp(method, "POST") == 0) {
        // Handle "/multiply" endpoint for multiplication
        if (strcmp(path, "/multiply") == 0) {
            // Extract the body content after the HTTP headers
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                body += 4; // Skip past the "\r\n\r\n" to reach the body

                int a = 0, b = 0;
                // Parse the parameters "a" and "b" from the request body
                if (sscanf(body, "a=%d&b=%d", &a, &b) == 2) {
                    char result[50];
                    snprintf(result, sizeof(result), "Result: %d", a * b);
                    send_response(client_socket, "200 OK", "text/plain", result);
                } else {
                    send_response(client_socket, "400 Bad Request", "text/plain", "Invalid parameters");
                }
            } else {
                send_response(client_socket, "400 Bad Request", "text/plain", "Bad Request");
            }
        } else {
            // Return 405 Method Not Allowed for unsupported POST endpoints
            send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method Not Allowed");
        }
    }
    // Handle DELETE requests
    else if (strcmp(method, "DELETE") == 0) {
        // Handle DELETE request for "/database.php?data=all"
        if (strcmp(path, "/database.php?data=all") == 0) {
            send_response(client_socket, "403 Forbidden", "text/plain", "Forbidden");
        } else {
            send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method Not Allowed");
        }
    } 
    // Handle unsupported HTTP methods
    else {
        send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method Not Allowed");
    }
    // Close the client socket after processing the request
    close(client_socket);
} // End of handle_request

// Worker function for handling client connections
void *worker_thread(void *arg) {
    while (1) {
        // Dequeue a client socket from the connection queue
        int client_socket = dequeue(&connection_queue);
        if (client_socket < 0) {
            continue;
        }
        // Handle the incoming HTTP request
        handle_request(client_socket);

        // Properly shut down the writing side of the socket
        shutdown(client_socket, SHUT_WR);

        // Close the client socket after ensuring all data is sent
        close(client_socket);
        printf("[INFO] Handled connection in thread %ld.\n", pthread_self());
    }
    return NULL;
} // End of worker thread

// Function to construct and initialize the server
struct Server server_constructor(int domain, int service, int protocol, uint32_t interface, int backlog) {
    struct Server server; // Declare a Server struct to hold server details

    // Configure the server struct with provided parameters
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.backlog = backlog;

    // Set up the server address structure
    server.address.sin_family = domain;              // Address family (IPv4)
    server.address.sin_port = htons(PORT);           // Convert port to network byte order
    server.address.sin_addr.s_addr = interface;      // Bind to the specified interface
    server.addrlen = sizeof(server.address);         // Size of the address structure

    // Create a socket using the specified domain, service, and protocol
    server.socket = socket(domain, service, protocol);
    if (server.socket < 0) { // Error handling if socket creation fails
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Socket created successfully.\n");

    // Set socket options to allow address reuse (prevents "address already in use" error)
    int opt = 1;
    if (setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server.socket);
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the specified IP address and port
    if (bind(server.socket, (struct sockaddr*)&server.address, sizeof(server.address)) < 0) {
        perror("Socket bind failed"); // Error handling if binding fails
        close(server.socket);
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Socket bound to port %d.\n", PORT);

    // Start listening for incoming connections with the specified backlog
    if (listen(server.socket, server.backlog) < 0) {
        perror("Listening failed"); // Error handling if listening fails
        close(server.socket);
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Listening for incoming connections...\n");

    return server; // Return the initialized server struct
} // End of server_constructor

int main() {
    // Register the signal handler for SIGINT (when you press Ctrl+C)
    // Doing this will makes sure the server shuts down gracefully by closing the socket
    signal(SIGINT, handle_sigint);

    // Create and initialize the server using the constructor function
    // AF_INET - IPv4, SOCK_STREAM - TCP, protocol 0 - default
    // INADDR_ANY - Bind to all available interfaces, backlog - max pending connections
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 10);
    
    // Store the server socket in a global variable for signal handling
    server_socket = server.socket;

    // Initialize the connection queue to manage incoming client connections
    init_queue(&connection_queue);

    // Create a pool of worker threads to handle incoming connections concurrently
    pthread_t threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        // Create a new thread and assign it to the worker function
        pthread_create(&threads[i], NULL, worker_thread, NULL);
    }

    // Main server loop to accept incoming connections indefinitely
    while (1) {
        printf("======== WAITING FOR CONNECTION ========\n");
        server.addrlen = sizeof(server.address);

        // Accept a new incoming connection, This is a blocking call that waits for a client to connect
        int new_socket = accept(server.socket, (struct sockaddr*)&server.address, (socklen_t*)&server.addrlen);
        
        // Check if the accept call was successful
        if (new_socket < 0) {
            perror("Accept failed"); // Print error if the accept call fails
            continue; // Retry accepting new connections if there's an error
        }

        printf("[INFO] Accepted a new connection.\n");

        // Add the new client socket to the connection queue,  Worker threads will pick up connections from this queue
        enqueue(&connection_queue, new_socket);
    }
    // Clean up the server socket before exiting (this code is typically unreachable)
    close(server.socket);
    return 0;
} // End of main function
