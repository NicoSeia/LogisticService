/**
 * @file server.hpp
 * @brief Declaration of the Server class for handling TCP/UDP client connections and server management.
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "alertHandler.hpp"
#include "anomalieHandler.hpp"
#include "errorHandler.hpp"
#include "lowStockChecker.hpp"
#include "orderStorage.hpp"
#include "orderValidation.hpp"
#include "json/allocator.h"
#include "json/assertions.h"
#include "json/config.h"
#include "json/forwards.h"
#include "json/json.h"
#include "json/json_features.h"
#include "json/reader.h"
#include "json/value.h"
#include "json/version.h"
#include "json/writer.h"
#include <algorithm>
#include <arpa/inet.h>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

/**
 * @brief Size of the server buffer.
 *
 * This macro defines the size of the buffer used by the server to handle
 * incoming and outgoing data.
 */
#define BUFFER_SIZE_SERVER 2048
/**
 * @brief Maximum number of client connections.
 *
 * This macro defines the maximum number of client connections allowed
 * by the server at any given time.
 */
#define MAX_CONNECTIONS 10

/**
 * @brief Maximum number of CLIENTs ID.
 *  
 */
#define MAX_CLIENTS_ID 10000

/**
 * @brief CHRONO TIMEOUT.
 *  
 */
#define CHRONO_TIMEOUT 60
#define CHRONO_TIMEOUT_SECONDS 30

/**
 * @brief Default port for the server.
 *
 * This macro defines the default port on which the server listens for incoming connections.
 */
#define PORT 8080

/**
* @struct ClientInfo
* @brief Structure to store information about connected clients.
*
* This structure holds details about a connected client, including their
* client ID, socket file descriptor, address information, protocol type (UDP/TCP),
* and the last time the client was active.
*/
struct ClientInfo
{
    int client_id;                /**< Unique identifier for the client. */
    int socket_fd;                /**< File descriptor for the client's socket. */
    struct sockaddr_in addr;      /**< Address information of the client. */
    std::string ip_address;       /**< IP address of the client. */
    std::string protocol;         /**< Protocol used by the client: "UDP" or "TCP". */
    std::chrono::steady_clock::time_point last_seen; /**< Timestamp of the last activity from the client. */
};

/**
* @var clientMapUdp
* @brief A map of UDP clients identified by their process ID (PID).
*
* This map stores information about UDP clients, where the key is the process ID (PID)
* and the value is the `ClientInfo` structure for the client.
*/
extern std::map<int, ClientInfo> clientMapUdp;

/**
* @var clientMapTcp
* @brief A map of TCP clients identified by their process ID (PID).
*
* This map stores information about TCP clients, where the key is the process ID (PID)
* and the value is the `ClientInfo` structure for the client.
*/
extern std::map<int, ClientInfo> clientMapTcp;

/**
* @class Server
* @brief A class to manage server functionality for handling TCP/UDP client connections.
*
* The Server class is responsible for configuring and managing TCP and UDP sockets, handling client requests,
* and maintaining a list of active clients. It also provides methods for client communication and server shutdown.
*/
class Server
{
  private:
    static Server* instance; /**< Singleton instance of the Server class. */
    int port;                /**< The port on which the server listens for incoming connections. */
    int socketUdpFd;         /**< File descriptor for the UDP socket. */
    int socketTcpFd;         /**< File descriptor for the TCP socket. */
    std::atomic<bool> running; /**< A flag indicating whether the server is running or not. */

    /**
    * @brief Private constructor for the Server class.
    *
    * Initializes the server with the specified port.
    * @param port The port on which the server will listen.
    */
    Server(int port);

    /**
    * @brief Configures the UDP socket for the server.
    * @param serv_addr The server address to bind the socket.
    * @param port The port to bind the socket.
    * @return The file descriptor for the configured UDP socket.
    */
    int socketUdpConfig(struct sockaddr_in serv_addr, int port);

    /**
    * @brief Configures the TCP socket for the server.
    * @param servaddr The server address to bind the socket.
    * @param port The port to bind the socket.
    * @return The file descriptor for the configured TCP socket.
    */
    int socketTcpConfig(struct sockaddr_in servaddr, int port);

    /**
    * @brief Handles TCP connections by accepting and processing client connections.
    */
    void handleTcpConnections();

    /**
    * @brief Handles communication with a TCP client.
    * @param client_sockfd The socket file descriptor for the client.
    * @param cli_addr The address of the client.
    */
    void handleTcpClient(int client_sockfd, struct sockaddr_in cli_addr);

  public:
    /**
    * @brief Gets the singleton instance of the Server class.
    * @param port The port to be used for the server instance.
    * @return The single instance of the Server class.
    */
    static Server* getInstance(int port);

    /**
    * @brief Handles communication with UDP clients.
    */
    void handleUdpClients();

    /**
    * @brief Starts the server, accepting and processing client connections.
    */
    void startServer();

    /**
    * @brief Stops the server from accepting new connections and shuts down active clients.
    */
    void stopServer();

    /**
    * @brief Closes the server sockets and releases resources.
    */
    void closeServer();

    /**
    * @brief Lists all connected clients.
    */
    static void listConnectedClients();

    /**
    * @brief Handles requests to list all connected clients.
    * @param protocol The protocol used ("UDP" or "TCP").
    * @param client_pid The client process ID requesting the list.
    */
    void handleListClientsRequest(const std::string& protocol, int client_pid);

    /**
    * @brief Handles requests to show a report based on a client's ID.
    * @param protocol The protocol used ("UDP" or "TCP").
    * @param client_id The client ID for which to show the report.
    */
    void handleShowReportRequest(const std::string& protocol, int client_id);

    /**
    * @brief Cleans up inactive UDP clients that have not sent messages within the given timeout.
    * @param timeout The time duration after which inactive clients are cleaned up.
    */
    void cleanupInactiveUdpClients(std::chrono::seconds timeout);

    /**
    * @brief Registers a new client with the server.
    * @param pid The process ID of the client.
    * @param protocol The protocol used by the client ("UDP" or "TCP").
    * @param addr The address information of the client.
    * @param socket_fd The socket file descriptor for the client.
    * @return The client ID assigned to the newly registered client.
    */
    int registerClient(int pid, const std::string& protocol, struct sockaddr_in addr, int socket_fd);

    /**
    * @brief Finds a client by their client ID and protocol.
    * @param clientId The client ID to search for.
    * @param protocol The protocol used by the client ("UDP" or "TCP").
    * @return A pointer to the `ClientInfo` structure for the client, or `nullptr` if not found.
    */
    ClientInfo* findClientById(int clientId, const std::string& protocol);

    /**
    * @brief Processes an incoming message from a client.
    * @param buffer The message buffer containing the client's message.
    * @param protocol The protocol used by the client ("UDP" or "TCP").
    * @param client_id The client ID from which the message was received.
    */
    void processMessage(char buffer[BUFFER_SIZE_SERVER], const std::string& protocol, int client_id);

    /**
    * @brief Forwards a message to a specific client.
    * @param message The message to send.
    * @param targetClientId The client ID to which the message should be sent.
    * @param protocol The protocol used by the target client ("UDP" or "TCP").
    */
    void forwardMessageToClient(const std::string& message, int targetClientId, const std::string& protocol);

    /**
    * @brief Prints the server's logo.
    * @return An integer indicating the success or failure of printing the logo.
    */
    int print_logo();

    /**
    * @brief Destructor for the Server class.
    *
    * Cleans up the resources and closes any open sockets.
    */
    ~Server();
};

#endif // SERVER_HPP
