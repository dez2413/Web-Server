// Queue.h
// Names: Desiree Gutierrez and Tyreke Hetzel
// Class: CS484 Computer Networking
// Date: 11/6/2024
// Purpose: This header file uses the structure and functions for a thread safe connection queue used by a multi-threaded HTTP server. 
// The connection queue is implemented using a circular buffer, allowing multiple threads to safely enqueue and dequeue client connections. 
// The queue uses a mutex for thread synchronization and a condition variable to signal when a new connection is added to the queue.

#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

#define QUEUE_SIZE 1000 // Maximum number of connections the queue can hold

// Define a structure for the connection queue
typedef struct {
    int sockets[QUEUE_SIZE];    // Array to store client socket file
    int front;                  // Index of the front element
    int rear;                   // Index of the rear element
    int count;                  // Current number of elements
    pthread_mutex_t mutex;      // Mutex for thread-safe access to the queue
    pthread_cond_t cond;        // Signal when the queue is not empty
} ConnectionQueue;

// Initializes the connection queue structure
void init_queue(ConnectionQueue *queue);

// Adds a client socket to the queue (thread-safe)
void enqueue(ConnectionQueue *queue, int client_socket);

// Removes and returns a client socket from the queue (thread-safe)
int dequeue(ConnectionQueue *queue);

#endif // end of QUEUE_H
