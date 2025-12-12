/**
 * @file client.h
 * @brief Header file for the client-side UDP communication module.
 *
 * This module provides functions to initialize a client, establish a connection
 * to a server using UDP, and handle communication by sending and receiving messages.
 * It also includes helper functions for processing JSON data.
 *
 * @author TryCatchFF
 * @date 2025-03-30
 */

#ifndef CLIENT_H
#define CLIENT_H

#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE_CLIENT 4096 ///< Buffer size for sending and receiving messages.
#define MAX_RETRIES 3           ///< Maximum number of retries before giving up on a response.
#define TIMEOUT 5               ///< Timeout duration in seconds for receiving a response.

struct receiver_data
{
    int sockfd;
    struct sockaddr_in* addr;
    const char* protocol;
};

/**
 * @brief Thread function that listens for incoming messages from other clients via server
 *
 * @param arg Pointer to receiver_data structure
 * @return void* NULL on completion
 */
void* receive_messages_thread(void* arg);

/**
 * @brief Start a background thread to receive messages from other clients via server
 *
 * @param sockfd Socket descriptor
 * @param addr Pointer to sockaddr_in structure
 * @param protocol Protocol used ("tcp" or "udp")
 * @return int 0 on success, -1 on failure
 */
int start_message_receiver(int sockfd, struct sockaddr_in* addr, const char* protocol);

/**
 * @brief Initializes the client by setting up the socket and server address.
 *
 * @param host Server hostname or IP address.
 * @param port Server port number.
 * @param sockfd Pointer to store the created socket file descriptor.
 * @param dest_addr Pointer to store the server's address information.
 * @param server Pointer to store the host information.
 * @return 0 on success, -1 on failure.
 */
int initialize_client_udp(char* host, int port, int* sockfd, struct sockaddr_in* dest_addr, struct hostent** server);

/**
 * @brief Manages communication with the server.
 *
 * Sends a JSON request to the server and waits for a response. Implements a retry
 * mechanism in case of no response.
 *
 * @param sockfd Socket file descriptor.
 * @param dest_addr Server address structure.
 * @return 0 on successful communication, 1 if max retries are reached,
 *         2 if the "close" message is received, -1 on error.
 */
int client_communicate_udp(int sockfd, struct sockaddr_in* dest_addr);

/**
 * @brief Handles communication with the server via TCP.
 *
 * @param sockfd The connected socket descriptor.
 * @return int 0 on success, -1 on failure.
 */
int client_communicate_tcp(int sockfd);

/**
 * @brief Initialize a TCP client socket.
 *
 * @param host The server hostname or IP.
 * @param port The server port number.
 * @param sockfd Pointer to the socket descriptor.
 * @param server_addr Pointer to sockaddr_in to be filled.
 * @param server Pointer to hostent structure to be filled.
 * @return int 0 on success, -1 on failure.
 */
int initialize_client_tcp(const char* host, int port, int* sockfd, struct sockaddr_in* server_addr,
                          struct hostent** server);

#endif // CLIENT_H
