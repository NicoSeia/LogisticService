#include "server.hpp"

Server* Server::instance = nullptr;
int nextClientId_udp = 1;
int nextClientId_tcp = 1;

std::map<int, ClientInfo> clientMapUdp; // PID -> ClientInfo
std::map<int, ClientInfo> clientMapTcp; // PID -> ClientInfo

Server::Server(int port)
{
    this->port = port;
    struct sockaddr_in servaddr;

    // Configurar ambos sockets
    socketUdpFd = socketUdpConfig(servaddr, port);
    socketTcpFd = socketTcpConfig(servaddr, port);

    print_logo();
}

Server* Server::getInstance(int port)
{
    if (!instance)
    {
        instance = new Server(port);
    }
    return instance;
}

void Server::startServer()
{
    running = true;
    // Crear hilos para manejar UDP y TCP simultáneamente
    std::thread udpThread(&Server::handleUdpClients, this);
    std::thread tcpThread(&Server::handleTcpConnections, this);

    // Esperar a que ambos hilos terminen (lo que no ocurrirá a menos que se cierre el servidor)
    udpThread.join();
    tcpThread.join();
}

ClientInfo* Server::findClientById(int clientId, const std::string& protocol)
{
    std::string proto_upper = protocol;
    std::transform(proto_upper.begin(), proto_upper.end(), proto_upper.begin(), ::toupper);

    if (proto_upper == "UDP")
    {
        for (auto& pair : clientMapUdp)
        {
            if (pair.second.client_id == clientId)
            {
                return &pair.second;
            }
        }
    }
    else if (proto_upper == "TCP")
    {
        for (auto& pair : clientMapTcp)
        {
            if (pair.second.client_id == clientId)
            {
                return &pair.second;
            }
        }
    }
    return nullptr;
}

void Server::forwardMessageToClient(const std::string& message, int targetClientId, const std::string& protocol)
{
    ClientInfo* targetClient = findClientById(targetClientId, protocol);

    if (!targetClient)
    {
        std::cerr << "Client ID " << targetClientId << " not found for protocol " << protocol << std::endl;
        return;
    }

    if (protocol == "udp")
    {
        socklen_t addr_size = sizeof(targetClient->addr);
        int n =
            sendto(socketUdpFd, message.c_str(), message.length(), 0, (struct sockaddr*)&targetClient->addr, addr_size);
        if (n < 0)
        {
            perror("ERROR forwarding message via UDP");
        }
        else
        {
            std::cout << "Message forwarded to UDP client #" << targetClientId << std::endl;
        }
    }
    else if (protocol == "tcp")
    {
        int n = write(targetClient->socket_fd, message.c_str(), message.length());
        if (n < 0)
        {
            perror("ERROR forwarding message via TCP");
        }
        else
        {
            std::cout << "Message forwarded to TCP client #" << targetClientId << std::endl;
        }
    }
    else
    {
        std::cerr << "Unknown protocol: " << protocol << std::endl;
    }
}

int Server::registerClient(int pid, const std::string& protocol, struct sockaddr_in addr, int socket_fd)
{

    // Check if the protocol is valid
    if (protocol != "UDP" && protocol != "TCP")
    {
        std::cerr << "Unknown protocol: " << protocol << std::endl;
        return -1;
    }

    // Check if the client is already registered
    if ((protocol == "UDP" && clientMapUdp.find(pid) != clientMapUdp.end()) ||
        (protocol == "TCP" && clientMapTcp.find(pid) != clientMapTcp.end()))
    {
        std::cerr << "Client with PID " << pid << " is already registered." << std::endl;
        return -1;
    }

    // Assign a new client ID
    int client_id = (protocol == "UDP") ? nextClientId_udp++ : nextClientId_tcp++;
    // Check if the client ID exceeds the maximum value
    if (client_id > MAX_CLIENTS_ID)
    {
        std::cerr << "Maximum client ID exceeded." << std::endl;
        return -1;
    }

    // Register the client
    ClientInfo info;
    info.client_id = client_id;
    info.socket_fd = socket_fd;
    info.addr = addr;
    info.ip_address = inet_ntoa(addr.sin_addr);
    info.protocol = protocol;
    info.last_seen = std::chrono::steady_clock::now();

    if (protocol == "UDP")
    {
        clientMapUdp[pid] = info;
    }
    else
    {
        clientMapTcp[pid] = info;
    }

    std::cout << "New " << protocol << " client #" << client_id << " connected with PID: " << pid << std::endl;

    return client_id;
}

void Server::cleanupInactiveUdpClients(std::chrono::seconds timeout)
{
    auto now = std::chrono::steady_clock::now();
    for (auto it = clientMapUdp.begin(); it != clientMapUdp.end();)
    {
        if (now - it->second.last_seen > timeout)
        {
            std::cout << "Removing inactive UDP client #" << it->second.client_id << std::endl;
            it = clientMapUdp.erase(it); // borra y avanza el iterador correctamente
        }
        else
        {
            ++it;
        }
    }
}

void Server::listConnectedClients()
{
    std::cout << "\n----- Connected Clients -----" << std::endl;
    std::cout << "UDP Clients: " << clientMapUdp.size() << std::endl;
    for (const auto& pair : clientMapUdp)
    {
        std::cout << "  Client #" << pair.second.client_id << " PID: " << pair.first
                  << " IP: " << pair.second.ip_address << std::endl;
    }

    std::cout << "TCP Clients: " << clientMapTcp.size() << std::endl;
    for (const auto& pair : clientMapTcp)
    {
        std::cout << "  Client #" << pair.second.client_id << " PID: " << pair.first
                  << " IP: " << pair.second.ip_address << std::endl;
    }
    std::cout << "----------------------------\n" << std::endl;
}

int Server::socketUdpConfig(struct sockaddr_in serv_addr, int port)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket UDP");
        exit(1);
    }

    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding UDP");
        exit(1);
    }

    std::cout << "UDP socket configured successfully on port " << port << std::endl;
    return sockfd;
}

int Server::socketTcpConfig(struct sockaddr_in servaddr, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int option = 1;
    if (sockfd < 0)
    {
        perror("Error creating TCP socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)))
    {
        perror("Error setting socket options");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, '0', sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons((uint16_t)port);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        perror("Error binding socket TCP");
        close(sockfd);
        return -1;
    }
    else
    {
        printf("TCP socket binded successfully on port %d\n", port);
    }

    if (listen(sockfd, MAX_CONNECTIONS) < 0)
    {
        perror("Error listening on socket");
        close(sockfd);
        return -1;
    }
    else
    {
        printf("TCP socket listening...\n");
    }

    return sockfd;
}

void Server::handleUdpClients()
{
    struct sockaddr_in cli_addr;
    socklen_t addr_size = sizeof(cli_addr);
    char buffer[BUFFER_SIZE_SERVER];
    int n;
    int client_id = 0;
    bool isValid = true;
    bool productStock = true;
    bool lowStock = false;
    bool reStocked = false;
    std::string alertOut;
    std::string errorMessage;
    mysqlx::Session session = connectToDb();

    while (running)
    {
        memset(buffer, 0, BUFFER_SIZE_SERVER);
        n = recvfrom(socketUdpFd, buffer, BUFFER_SIZE_SERVER - 1, 0, (struct sockaddr*)&cli_addr, &addr_size);

        if (n < 0)
        {
            perror("ERROR en recvfrom UDP");
            continue;
        }

        int client_pid = atoi(buffer);

        if (clientMapUdp.find(client_pid) == clientMapUdp.end() && client_pid != 0)
        {
            client_id = registerClient(client_pid, "UDP", cli_addr, 0);
            continue;
        }

        std::string response = "UDP Server received your message.";
        n = sendto(socketUdpFd, response.c_str(), response.size(), 0, (struct sockaddr*)&cli_addr, addr_size);
        if (n < 0)
        {
            perror("ERROR in sendto UDP");
        }

        if (buffer[0] == '{')
        {
            storeOrder(buffer);

            // --- JSON parsing ---
            Json::CharReaderBuilder builder;
            Json::CharReader* reader = builder.newCharReader();
            Json::Value root;
            std::string parseErrors;

            bool parsingSuccessful = reader->parse(buffer, buffer + strlen(buffer), &root, &parseErrors);
            delete reader;

            if (!parsingSuccessful)
            {
                std::cout << "Error parsing JSON: " << parseErrors << std::endl;
                continue; // salta a la próxima iteración
            }

            isValid = validateOrderLimits(root, errorMessage);
            if (!isValid)
            {
                std::cout << "\n\nError validating order limits: " << errorMessage << std::endl;
                n = sendto(socketUdpFd, errorMessage.c_str(), errorMessage.size(), 0, (struct sockaddr*)&cli_addr,
                           addr_size);
                if (n < 0)
                {
                    perror("ERROR in sendto UDP");
                }
            }

            productStock = checkProductStock(root, errorMessage, session);
            if (!productStock)
            {
                std::cout << "\n\nError checking product stock: " << errorMessage << std::endl;
                n = sendto(socketUdpFd, errorMessage.c_str(), errorMessage.size(), 0, (struct sockaddr*)&cli_addr,
                           addr_size);
                if (n < 0)
                {
                    perror("ERROR in sendto UDP");
                }
            }

            if (productStock && isValid)
            {
                int result = realTimeUpdate(session, root);
                if (result > 0)
                {
                    std::string orderSuccess = "Successful order!";
                    n = sendto(socketUdpFd, orderSuccess.c_str(), orderSuccess.size(), 0, (struct sockaddr*)&cli_addr,
                               addr_size);
                    if (n < 0)
                    {
                        perror("ERROR in sendto UDP");
                    }
                }
                else
                {
                    std::cout << "❌ Error updating inventory." << std::endl;
                }
            }

            // Check for low stock
            lowStock = checkLowStockAlert(session, root, alertOut);
            if (lowStock)
            {
                std::cout << "\n\nLow stock alert: " << alertOut << std::endl;
                n = sendto(socketUdpFd, alertOut.c_str(), alertOut.size(), 0, (struct sockaddr*)&cli_addr, addr_size);
                if (n < 0)
                {
                    perror("ERROR in sendto UDP");
                }
            }

            // Check for re-stock
            reStocked = reStock(session, root, alertOut);
            if (reStocked)
            {
                std::cout << "\n\nRe-stock alert: " << alertOut << std::endl;
                n = sendto(socketUdpFd, alertOut.c_str(), alertOut.size(), 0, (struct sockaddr*)&cli_addr, addr_size);
                if (n < 0)
                {
                    perror("ERROR in sendto UDP");
                }
            }
        }
        if (isValid && productStock)
        {
            processMessage(buffer, "UDP", client_id);
        }
    }
}

void Server::handleTcpConnections()
{

    while (running)
    {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);

        // Aceptar nueva conexión TCP
        int client_sockfd = accept(socketTcpFd, (struct sockaddr*)&cli_addr, &cli_len);
        if (client_sockfd < 0)
        {
            perror("ERROR on accepting TCP connection");
            continue;
        }

        // Crear un hilo para manejar este cliente TCP específico
        std::thread clientThread(&Server::handleTcpClient, this, client_sockfd, cli_addr);
        clientThread.detach(); // Permitir que el hilo se ejecute independientemente
    }
}

void Server::handleTcpClient(int client_sockfd, struct sockaddr_in cli_addr)
{
    char buffer[BUFFER_SIZE_SERVER];
    int n;
    int client_id = 0;
    bool isValid = true;
    bool productStock = true;
    bool lowStock = false;
    bool reStocked = false;
    std::string alertOut;
    std::string errorMessage;
    mysqlx::Session session = connectToDb();

    // Primera recepción para obtener el PID
    memset(buffer, 0, BUFFER_SIZE_SERVER);
    n = read(client_sockfd, buffer, BUFFER_SIZE_SERVER - 1);

    if (n <= 0)
    {
        close(client_sockfd);
        return;
    }

    int client_pid = atoi(buffer);
    if (client_pid != 0)
    {
        client_id = registerClient(client_pid, "TCP", cli_addr, client_sockfd);
    }

    // Loop para recibir mensajes de este cliente
    while (running)
    {
        memset(buffer, 0, BUFFER_SIZE_SERVER);
        n = read(client_sockfd, buffer, BUFFER_SIZE_SERVER - 1);

        if (n <= 0)
        {
            // Cliente desconectado o error
            std::cout << "TCP client disconnected (PID: " << client_pid << ")" << std::endl;
            close(client_sockfd);
            // Eliminar cliente de la lista
            if (client_pid != 0)
            {
                clientMapTcp.erase(client_pid);
            }
            listConnectedClients();
            break;
        }

        // Respond to the client first
        std::string response = "TCP Server received your message.";
        int written = write(client_sockfd, response.c_str(), response.size());
        if (written < 0)
        {
            perror("ERROR writing to TCP socket");
            continue;
        }

        if (buffer[0] == '{')
        {
            storeOrder(buffer);

            // --- JSON parsing ---
            Json::CharReaderBuilder builder;
            Json::CharReader* reader = builder.newCharReader();
            Json::Value root;
            std::string parseErrors;

            bool parsingSuccessful = reader->parse(buffer, buffer + strlen(buffer), &root, &parseErrors);
            delete reader;

            if (!parsingSuccessful)
            {
                std::cout << "Error parsing JSON: " << parseErrors << std::endl;
                continue; // salta a la próxima iteración
            }

            isValid = validateOrderLimits(root, errorMessage);
            if (!isValid)
            {
                std::cout << "\n\nError validating order limits: " << errorMessage << std::endl;
                int written = write(client_sockfd, errorMessage.c_str(), errorMessage.size());
                if (written < 0)
                {
                    perror("ERROR writing to TCP socket valid order");
                    continue;
                }
            }

            productStock = checkProductStock(root, errorMessage, session);
            if (!productStock)
            {
                std::cout << "\n\nError checking product stock: " << errorMessage << std::endl;
                int written = write(client_sockfd, errorMessage.c_str(), errorMessage.size());
                if (written < 0)
                {
                    perror("ERROR writing to TCP socket check stock");
                    continue;
                }
            }

            if (productStock && isValid)
            {
                int result = realTimeUpdate(session, root);
                if (result > 0)
                {
                    std::string orderSuccess = "Successful order!";
                    int written = write(client_sockfd, orderSuccess.c_str(), orderSuccess.size());
                    if (written < 0)
                    {
                        perror("ERROR writing to TCP socket order success");
                        continue;
                    }
                }
                else
                {
                    std::cout << "❌ Error updating inventory." << std::endl;
                }
            }

            // Check for low stock
            lowStock = checkLowStockAlert(session, root, alertOut);
            if (lowStock)
            {
                std::cout << "\n\nLow stock alert: " << alertOut << std::endl;
                int written = write(client_sockfd, alertOut.c_str(), alertOut.size());
                if (written < 0)
                {
                    perror("ERROR writing to TCP socket low stock");
                    continue;
                }
            }

            // Check for re-stock
            reStocked = reStock(session, root, alertOut);
            if (reStocked)
            {
                std::cout << "\n\nRe-stock alert: " << alertOut << std::endl;
                int written = write(client_sockfd, alertOut.c_str(), alertOut.size());
                if (written < 0)
                {
                    perror("ERROR writing to TCP socket re stock");
                    continue;
                }
            }
        }
        if (isValid && productStock)
        {
            processMessage(buffer, "TCP", client_id);
        }
    }
}

void Server::handleListClientsRequest(const std::string& protocol, int client_id)
{
    std::ostringstream msgStream;

    msgStream << "\n----- Connected Clients -----\n";

    msgStream << "UDP Clients: " << clientMapUdp.size() << "\n";
    for (const auto& pair : clientMapUdp)
    {
        msgStream << "  Client #" << pair.second.client_id << " PID: " << pair.first
                  << " IP: " << pair.second.ip_address << "\n";
    }

    msgStream << "TCP Clients: " << clientMapTcp.size() << "\n";
    for (const auto& pair : clientMapTcp)
    {
        msgStream << "  Client #" << pair.second.client_id << " PID: " << pair.first
                  << " IP: " << pair.second.ip_address << "\n";
    }

    msgStream << "----------------------------";

    std::string response = msgStream.str();

    std::cout << "\nReceived LIST_CLIENTS command via " << protocol << " from ID " << client_id << "." << std::endl;
    std::cout << response << std::endl;
    std::cout << "Sending message of length: " << response.length() << " bytes." << std::endl;

    // Enviar respuesta al cliente que hizo la solicitud
    forwardMessageToClient(response, client_id, protocol);
}

void Server::handleShowReportRequest(const std::string& protocol, int client_id)
{
    std::ostringstream msgStream;
    std::lock_guard<std::mutex> lock(ordersMutex);

    if (productQuantities.empty())
    {
        msgStream << "No product data stored yet.\n";
    }
    else
    {
        msgStream << "\n----- Product Quantity Report -----\n";
        for (const auto& pair : productQuantities)
        {
            msgStream << "- " << pair.first << ": " << pair.second << "\n";
        }
        msgStream << "-----------------------------------";
    }

    std::string response = msgStream.str();

    std::cout << "\nReceived SHOW_REPORT command via " << protocol << " from ID " << client_id << "." << std::endl;
    std::cout << response << std::endl;
    std::cout << "Sending message of length: " << response.length() << " bytes." << std::endl;

    forwardMessageToClient(response, client_id, protocol);
}

void Server::processMessage(char buffer[BUFFER_SIZE_SERVER], const std::string& protocol, int client_id)
{
    std::string msg(buffer);

    if (msg == "LIST_CLIENTS")
    {
        std::string lower_protocol = protocol;
        std::transform(lower_protocol.begin(), lower_protocol.end(), lower_protocol.begin(), ::tolower);

        handleListClientsRequest(lower_protocol, client_id);
        return;
    }

    if (msg == "SHOW_REPORT")
    {
        std::string lower_protocol = protocol;
        std::transform(lower_protocol.begin(), lower_protocol.end(), lower_protocol.begin(), ::tolower);

        handleShowReportRequest(lower_protocol, client_id);
        return;
    }

    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    std::istringstream s(msg);
    if (Json::parseFromStream(readerBuilder, s, &root, &errs))
    {
        if (root.isMember("general_info"))
        {
            const Json::Value& general_info = root["general_info"];
            std::string id = general_info["id"].asString();
            std::string source_type = general_info["source"]["type"].asString();
            int source_location = general_info["source"]["location"].asInt();
            std::string destination_type = general_info["destination"]["type"].asString();
            int destination_location = general_info["destination"]["location"].asInt();
            std::string action_type = general_info["action"]["type"].asString();
            std::string product_id = general_info["action"]["product"]["id"].asString();
            std::string product_name = general_info["action"]["product"]["name"].asString();
            int product_quantity = general_info["action"]["product"]["quantity"].asInt();
            std::string date = general_info["metadata"]["date"].asString();
            std::string message = general_info["metadata"]["message"].asString();
            std::string priority = general_info["metadata"]["priority"].asString();
            std::string protocol_destination = general_info["metadata"]["protocol"].asString();

            std::cout << "\n------------- Received " << protocol << " data --------------" << std::endl;
            std::cout << "ID: " << id << std::endl;
            std::cout << "Source Type: " << source_type << std::endl;
            std::cout << "Source Location: " << source_location << std::endl;
            std::cout << "Destination Type: " << destination_type << std::endl;
            std::cout << "Destination Location: " << destination_location << std::endl;
            std::cout << "Action Type: " << action_type << std::endl;
            std::cout << "Product: " << product_id << " " << product_name << std::endl;
            std::cout << "Product Quantity: " << product_quantity << std::endl;
            std::cout << "Message: " << message << std::endl;
            std::cout << "Priority: " << priority << std::endl;
            std::cout << "Date: " << date << std::endl;
            std::cout << "------------------------------------------\n" << std::endl;

            std::string msj_forward =
                "FORWARDED_MESSAGE: " + action_type + " " + std::to_string(product_quantity) + " " + product_name;
            std::cout << "Forwarding message to client #" << source_location << " via " << protocol_destination
                      << std::endl;
            std::cout << "Message: " << msj_forward << std::endl;
            std::string lower_protocol = protocol_destination;
            std::transform(lower_protocol.begin(), lower_protocol.end(), lower_protocol.begin(), ::tolower);
            forwardMessageToClient(msj_forward, source_location, lower_protocol);
        }
        else
        {
            std::cout << "No 'general_info' key found in JSON via " << protocol << "." << std::endl;
        }
    }
    else
    {
        std::cout << "Error parsing JSON via " << protocol << ": " << errs << std::endl;
    }
}

int Server::print_logo()
{
    std::cout << "###################################################################################################"
              << std::endl;
    std::cout << "###################################################################################################"
              << std::endl;
    std::cout << "                                                                                                   "
              << std::endl;
    std::cout << "                  00000000000000           000000000        00000000  00000000 " << std::endl;
    std::cout << "                       000                00        00      00        00       " << std::endl;
    std::cout << "                       000               000                00        00       " << std::endl;
    std::cout << "                       000     0000000   000                00000000  00000000 " << std::endl;
    std::cout << "                       000     0000000   000                00        00 " << std::endl;
    std::cout << "                       000               000                00        00 " << std::endl;
    std::cout << "                       000                00        00      00        00 " << std::endl;
    std::cout << "                       000                 000000000        00        00" << std::endl;
    std::cout << "                                                                                                  "
              << std::endl;
    std::cout << "                           -=  TECHNOLOGY FOR A BETTER FUTURE  =-                                 "
              << std::endl;
    std::cout << "###################################################################################################"
              << std::endl;
    std::cout << "                          Vault-Tec Corporation. ALL RIGHTS RESERVED                               "
              << std::endl;
    std::cout << "###################################################################################################"
              << std::endl;
    std::cout << "                                   >>> SERVER ONLINE <<<                                           "
              << std::endl;
    std::cout << "###################################################################################################"
              << std::endl;
    return 0;
}

Server::~Server()
{
    close(socketUdpFd);
    close(socketTcpFd);
}

void Server::stopServer()
{
    running = false;
    close(socketUdpFd);
    close(socketTcpFd);
}

void Server::closeServer()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}