/**
 * @file client.c
 * @brief Implementation of the client handling communication.
 */

#include "client.h"

/**
 * @brief Thread function that listens for incoming messages from other clients via server
 *
 * @param arg Pointer to receiver_data structure
 * @return void* NULL on completion
 */
void* receive_messages_thread(void* arg)
{
    struct receiver_data* data = (struct receiver_data*)arg;
    int sockfd = data->sockfd;
    struct sockaddr_in* addr = data->addr;
    const char* protocol = data->protocol;

    char buffer[BUFFER_SIZE_CLIENT];
    int addr_size = sizeof(*addr);

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE_CLIENT);

        if (strcmp(protocol, "udp") == 0)
        {
            // UDP receiving logic
            int n = recvfrom(sockfd, buffer, BUFFER_SIZE_CLIENT - 1, 0, (struct sockaddr*)addr, &addr_size);

            if (n < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // Timeout, continue listening
                    continue;
                }
                else
                {
                    perror("ERROR in receive_messages_thread recvfrom");
                    break;
                }
            }

            // Process received message
            buffer[n] = '\0';

            // Check if it's a forwarded message from another client
            if (strstr(buffer, "FORWARDED_MESSAGE: ") != NULL)
            {
                printf("\n\n[INCOMING MESSAGE]: %s\n", buffer);
            }
            else
            {
                printf("\n\n[SERVER MESSAGE]: %s\n", buffer);
            }

            printf("Your input > ");
            fflush(stdout);
        }
        else if (strcmp(protocol, "tcp") == 0)
        {
            // TCP receiving logic - we'll use recv with MSG_DONTWAIT for non-blocking
            int n = recv(sockfd, buffer, BUFFER_SIZE_CLIENT - 1, MSG_DONTWAIT);

            if (n < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // No data available, continue listening
                    sleep(1); // Sleep to prevent CPU hogging
                    continue;
                }
                else
                {
                    perror("ERROR in receive_messages_thread recv");
                    break;
                }
            }
            else if (n == 0)
            {
                // Connection closed by server
                printf("\nServer closed connection.\n");
                break;
            }

            // Process received message
            buffer[n] = '\0';

            // Check if it's a forwarded message from another client
            if (strstr(buffer, "FORWARDED_MESSAGE: ") != NULL)
            {
                printf("\n\n[INCOMING MESSAGE]: %s\n", buffer);
            }
            else
            {
                printf("\n\n[SERVER MESSAGE]: %s\n", buffer);
            }
            printf("Your input > ");
            fflush(stdout);
        }
    }

    free(data); // Free the allocated structure
    return NULL;
}

/**
 * @brief Start a background thread to receive messages from other clients via server
 *
 * @param sockfd Socket descriptor
 * @param addr Pointer to sockaddr_in structure
 * @param protocol Protocol used ("tcp" or "udp")
 * @return int 0 on success, -1 on failure
 */
int start_message_receiver(int sockfd, struct sockaddr_in* addr, const char* protocol)
{
    pthread_t receiver_thread;
    struct receiver_data* data = malloc(sizeof(struct receiver_data));

    if (!data)
    {
        perror("ERROR allocating memory for receiver data");
        return -1;
    }

    data->sockfd = sockfd;
    data->addr = addr;
    data->protocol = protocol;

    // For UDP, set socket timeout to prevent blocking in recvfrom
    if (strcmp(protocol, "udp") == 0)
    {
        struct timeval tv;
        tv.tv_sec = 1; // 1 second timeout
        tv.tv_usec = 0;

        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0)
        {
            perror("ERROR setting socket timeout option");
            free(data);
            return -1;
        }
    }

    // Create thread to listen for incoming messages
    if (pthread_create(&receiver_thread, NULL, receive_messages_thread, data) != 0)
    {
        perror("ERROR creating receiver thread");
        free(data);
        return -1;
    }

    // Detach thread so it can clean up after itself
    pthread_detach(receiver_thread);

    return 0;
}

// Initializes the client (socket, address, port)
int initialize_client_udp(char* host, int port, int* sockfd, struct sockaddr_in* dest_addr, struct hostent** server)
{
    *server = gethostbyname(host);
    if (*server == NULL)
    {
        fprintf(stderr, "ERROR: Could not get the address of the server %s\n", host);
        return -1;
    }

    *sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sockfd < 0)
    {
        perror("ERROR in opening socket");
        return -1;
    }

    dest_addr->sin_family = AF_INET;
    dest_addr->sin_port = htons(port);
    dest_addr->sin_addr = *((struct in_addr*)((*server)->h_addr_list[0]));
    memset(&(dest_addr->sin_zero), '\0', 8);

    pid_t client_pid = getpid();
    char cl_pid_str[6];
    sprintf(cl_pid_str, "%d", client_pid);
    if (sendto(*sockfd, cl_pid_str, sizeof(cl_pid_str), 0, (struct sockaddr*)dest_addr, sizeof(*dest_addr)) < 0)
    {
        perror("ERROR while sending client PID");
        close(*sockfd);
        return -1;
    }
    else
    {
        printf("Client PID %d sent successfully\n", client_pid);
    }
    printf("Client PID %d\n", client_pid);

    if (start_message_receiver(*sockfd, dest_addr, "udp") != 0)
    {
        fprintf(stderr, "ERROR: Could not start message receiver\n");
        close(*sockfd);
        return -1;
    }

    return 0;
}

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
                          struct hostent** server)
{
    *server = gethostbyname(host);
    if (*server == NULL)
    {
        fprintf(stderr, "ERROR: No such host: %s\n", host);
        return -1;
    }

    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sockfd < 0)
    {
        perror("ERROR opening socket");
        return -1;
    }

    memset(server_addr, 0, sizeof(struct sockaddr_in));
    server_addr->sin_family = AF_INET;
    memcpy(&server_addr->sin_addr.s_addr, (*server)->h_addr, (*server)->h_length);
    server_addr->sin_port = htons(port);

    if (connect(*sockfd, (struct sockaddr*)server_addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("ERROR connecting");
        close(*sockfd);
        return -1;
    }

    // Enviar PID al servidor después de conectarse, como en la versión UDP
    pid_t client_pid = getpid();
    char cl_pid_str[6];
    sprintf(cl_pid_str, "%d", client_pid);

    if (send(*sockfd, cl_pid_str, strlen(cl_pid_str), 0) < 0)
    {
        perror("ERROR while sending client PID");
        close(*sockfd);
        return -1;
    }
    else
    {
        printf("Client PID %d sent successfully\n", client_pid);
    }
    printf("Client PID %d\n", client_pid);

    if (start_message_receiver(*sockfd, server_addr, "tcp") != 0)
    {
        fprintf(stderr, "ERROR: Could not start message receiver\n");
        close(*sockfd);
        return -1;
    }

    return 0;
}

// Manages communication with the server (sending and receiving)
int client_communicate_udp(int sockfd, struct sockaddr_in* dest_addr)
{
    char buffer_send[BUFFER_SIZE_CLIENT];
    char buffer_received[BUFFER_SIZE_CLIENT];
    int addr_size, n, retries = 0;

    cJSON* json = read_json_from_file("../config/request_format.json");

    if (json == NULL)
    {
        fprintf(stderr, "ERROR: Could not read JSON file\n");
        exit(1);
    }

    memset(buffer_send, 0, BUFFER_SIZE_CLIENT);
    char buffer[BUFFER_SIZE_CLIENT];
    update_json_with_user_input(json, buffer);
    char* json_string = cJSON_Print(json);
    if (json_string == NULL)
    {
        fprintf(stderr, "ERROR: Could not print JSON\n");
        cJSON_Delete(json);
        return -1;
    }
    strncpy(buffer_send, json_string, BUFFER_SIZE_CLIENT - 1);
    buffer_send[BUFFER_SIZE_CLIENT - 1] = '\0'; // Ensure null termination
    cJSON_Delete(json);
    free(json_string);

    addr_size = sizeof(*dest_addr);
    while (retries <= MAX_RETRIES)
    {

        n = sendto(sockfd, (void*)buffer_send, BUFFER_SIZE_CLIENT, 0, (struct sockaddr*)dest_addr, addr_size);
        if (n < 0)
        {
            perror("ERROR in sendto");
            return -1;
        }

        // Clear the buffer for the response
        memset(buffer_received, 0, sizeof(buffer_received));

        // Use select to wait for the response with a timeout
        fd_set read_fds;
        struct timeval timeout;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);

        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        int ret = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);

        if (ret == -1)
        {
            perror("ERROR in select");
            return -1;
        }
        else if (ret == 0)
        {
            printf("The server did not respond within the expected time.\n");
            retries++;
            sleep(10); // Wait before retrying
            printf("Retrying... (%d/%d)\n", retries, MAX_RETRIES);
        }
        else
        {

            n = recvfrom(sockfd, (void*)buffer_received, BUFFER_SIZE_CLIENT, 0, (struct sockaddr*)dest_addr,
                         &addr_size);
            if (n < 0)
            {
                perror("ERROR in recvfrom");
                return -1;
            }
            printf("\nResponse: %s\n\n", buffer_received);

            return 0; // Successful communication
        }
    }
    printf("Max retries reached. Server may be down.\n");
    return 1; // Exceeded max retries
}

/**
 * @brief Handles communication with the server via TCP.
 *
 * @param sockfd The connected socket descriptor.
 * @return int 0 on success, -1 on failure.
 */
int client_communicate_tcp(int sockfd)
{
    char buffer_send[BUFFER_SIZE_CLIENT];
    char buffer_received[BUFFER_SIZE_CLIENT];
    int retries = 0;

    cJSON* json = read_json_from_file("../config/request_format.json");
    if (json == NULL)
    {
        fprintf(stderr, "ERROR: Could not read JSON file\n");
        return -1;
    }

    // Armar JSON con entrada del usuario
    char buffer[BUFFER_SIZE_CLIENT];
    update_json_with_user_input(json, buffer);
    char* json_string = cJSON_Print(json);
    if (json_string == NULL)
    {
        fprintf(stderr, "ERROR: Could not serialize JSON\n");
        cJSON_Delete(json);
        return -1;
    }

    strncpy(buffer_send, json_string, BUFFER_SIZE_CLIENT - 1);
    buffer_send[BUFFER_SIZE_CLIENT - 1] = '\0';

    // Liberar memoria
    cJSON_Delete(json);
    free(json_string);

    while (retries <= MAX_RETRIES)
    {
        // Enviar JSON
        int n = send(sockfd, buffer_send, strlen(buffer_send), 0);
        if (n < 0)
        {
            perror("ERROR sending data");
            return -1;
        }

        // Configurar timeout para recibir respuesta
        struct timeval tv;
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;

        // Establecer timeout para el socket
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0)
        {
            perror("ERROR setting socket timeout option");
            return -1;
        }

        // Esperar respuesta con timeout
        memset(buffer_received, 0, BUFFER_SIZE_CLIENT);
        n = recv(sockfd, buffer_received, BUFFER_SIZE_CLIENT - 1, 0);
        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // Timeout ocurrió
                printf("The server did not respond within the expected time.\n");
                retries++;
                sleep(10); // Esperar antes de reintentar
                printf("Retrying... (%d/%d)\n", retries, MAX_RETRIES);
                continue;
            }
            else
            {
                perror("ERROR receiving data");
                return -1;
            }
        }

        buffer_received[n] = '\0'; // Asegurar null terminator
        printf("\nResponse: %s\n\n", buffer_received);
        return 0; // Comunicación exitosa
    }

    printf("Max retries reached. Server may be down.\n");
    return 1; // Se excedieron los reintentos máximos
}
