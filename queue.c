// Queue.c
// Names: Desiree Gutierrez and Tyreke Hetzel
// Class: CS484 Computer Networking
// Date: 11/6/2024
// Purpose: This file implements the functions for managing a thread-safe connection queue used in a multi-threaded HTTP server. 
// The queue is implemented using a circular buffer with synchronization mechanisms (mutex and condition variables) to ensure safe concurrent access by multiple threads.

#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

// Initializes the connection queue by setting the front, rear, and count to 0. It also initializes the mutex and condition variable for thread synchronization.
void init_queue(ConnectionQueue *queue) {
    queue->front = 0;  // Initialize the front index to 0
    queue->rear = 0;   // Initialize the rear index to 0
    queue->count = 0;  // Set the initial count of elements in the queue to 0
    pthread_mutex_init(&queue->mutex, NULL); // Initialize the mutex
    pthread_cond_init(&queue->cond, NULL);   // Initialize the condition variable
} // End of init queue

// Adds a new client socket to the rear of the queue in a thread safe manner. If the queue is full, the function waits until space is available.
void enqueue(ConnectionQueue *queue, int client_socket) {
    pthread_mutex_lock(&queue->mutex); // Lock the mutex to make sure it has exclusive access to the queue

    // Wait if the queue is full (count equals QUEUE_SIZE)
    while (queue->count == QUEUE_SIZE) {
        pthread_cond_wait(&queue->cond, &queue->mutex); // Wait for a signal when that space is available
    }

    // Add the client socket to the queue at the rear position
    queue->sockets[queue->rear] = client_socket;

    // Update the rear index using modulo to wrap around if it reaches the end of the array
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->count++; // Increment the count of elements in the queue

    // Signal a waiting thread that a new item is available
    pthread_cond_signal(&queue->cond);

    pthread_mutex_unlock(&queue->mutex); // Unlock the mutex
} // End of enqueue

// Removes and returns a client socket from the front of the queue in a thread safe manner. If the queue is empty, the function waits until a new connection is enqueued.
int dequeue(ConnectionQueue *queue) {
    pthread_mutex_lock(&queue->mutex); // Lock the mutex to make sure it has exclusive access to the queue

    // Wait if the queue is empty (count equals 0)
    while (queue->count == 0) {
        pthread_cond_wait(&queue->cond, &queue->mutex); // Wait for a signal that an item is available
    }

    // Retrieve the client socket from the front of the queue
    int client_socket = queue->sockets[queue->front];

    // Update the front index using modulo to wrap around if it reaches the end of the array
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->count--; // Subtract the count of elements in the queue

    // Signal a waiting thread that space is now available
    pthread_cond_signal(&queue->cond);

    pthread_mutex_unlock(&queue->mutex); // Unlock the mutex

    return client_socket; // Return the dequeued client socket
} // End of dequeue
