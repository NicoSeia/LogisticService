#include "testServer.hpp"

static Server* server = nullptr;

// Función global para limpiar el estado del servidor antes de cada test
void resetServerState()
{
    if (!server)
        return;
    clientMapUdp.clear();
    clientMapTcp.clear();
    // También podrías limpiar otras estructuras si el servidor tiene más
}

// Mock ClientInfo for testing
ClientInfo createMockClientInfo(int clientId, const std::string& protocol, const std::string& ipAddress)
{
    ClientInfo client;
    client.client_id = clientId;
    client.protocol = protocol;
    client.ip_address = ipAddress;
    client.last_seen = std::chrono::steady_clock::now();
    return client;
}

// Test for registering a client
TEST(ServerTests, RegisterClient)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &mockAddr.sin_addr);

    server->registerClient(1234, "UDP", mockAddr, 0);

    ASSERT_EQ(clientMapUdp.size(), 1);
    ASSERT_EQ(clientMapUdp[1234].client_id, 1);
    ASSERT_EQ(clientMapUdp[1234].protocol, "UDP");
    ASSERT_EQ(clientMapUdp[1234].ip_address, "127.0.0.1");
}

TEST(ServerTests, FindClientById)
{
    resetServerState();

    ClientInfo mockClient = createMockClientInfo(1, "UDP", "127.0.0.1");
    clientMapUdp[1234] = mockClient;

    ClientInfo* foundClient = server->findClientById(1, "UDP");
    ASSERT_NE(foundClient, nullptr);
    ASSERT_EQ(foundClient->client_id, 1);
    ASSERT_EQ(foundClient->protocol, "UDP");
    ASSERT_EQ(foundClient->ip_address, "127.0.0.1");
}

TEST(ServerTests, CleanupInactiveUdpClients)
{
    resetServerState();

    ClientInfo mockClient = createMockClientInfo(1, "UDP", "127.0.0.1");
    mockClient.last_seen -= std::chrono::seconds(10); // Simulate inactivity
    clientMapUdp[1234] = mockClient;

    server->cleanupInactiveUdpClients(std::chrono::seconds(5));

    ASSERT_EQ(clientMapUdp.size(), 0);
}

TEST(ServerTests, ListConnectedClients)
{
    resetServerState();

    ClientInfo mockClientUdp = createMockClientInfo(1, "UDP", "127.0.0.1");
    ClientInfo mockClientTcp = createMockClientInfo(2, "TCP", "192.168.1.1");
    clientMapUdp[1234] = mockClientUdp;
    clientMapTcp[5678] = mockClientTcp;

    testing::internal::CaptureStdout();
    server->listConnectedClients();
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_NE(output.find("UDP Clients: 1"), std::string::npos);
    ASSERT_NE(output.find("TCP Clients: 1"), std::string::npos);
    ASSERT_NE(output.find("Client #1"), std::string::npos);
    ASSERT_NE(output.find("Client #2"), std::string::npos);
}

TEST(ServerTests, ProcessMessageInvalidJson)
{
    resetServerState();

    char buffer[BUFFER_SIZE_SERVER_T] = R"({ invalid_json })";

    testing::internal::CaptureStdout();
    server->processMessage(buffer, "UDP", 0);
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_NE(output.find("Error parsing JSON via UDP"), std::string::npos);
}

TEST(ServerTests, ProcessMessageValidJson)
{
    resetServerState();

    char buffer[BUFFER_SIZE_SERVER_T] = R"({
        "general_info": {
            "id": "1234",
            "source": {"type": "UDP", "location": 0},
            "destination": {"type": "TCP", "location": 1},
            "action": {
                "type": "SEND",
                "product": {"id": "P001", "name": "Product1", "quantity": 10}
            },
            "metadata": {
                "date": "2023-10-01",
                "message": "Test message",
                "priority": "high",
                "protocol": "TCP"
            }
        }
    })";

    testing::internal::CaptureStdout();
    server->processMessage(buffer, "UDP", 0);
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_NE(output.find("Received UDP data"), std::string::npos);
    ASSERT_NE(output.find("Forwarding message to client #0 via TCP"), std::string::npos);
}

TEST(ServerTests, ForwardMessageToTcpClient)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.0.10", &mockAddr.sin_addr);

    // Simulamos que el cliente TCP tiene un socket ficticio 123
    server->registerClient(2222, "TCP", mockAddr, 123);

    // Mock del write() usando pipe para no fallar
    int fds[2];
    pipe(fds);
    clientMapTcp[2222].socket_fd = fds[1];

    testing::internal::CaptureStdout();
    server->forwardMessageToClient("Test TCP Message", 1, "tcp");
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_NE(output.find("Message forwarded to TCP client #1"), std::string::npos);

    close(fds[0]);
    close(fds[1]);
}

TEST(ServerTests, ForwardMessageToNonExistentClient)
{
    resetServerState();

    testing::internal::CaptureStderr();
    server->forwardMessageToClient("Test", 999, "UDP");
    std::string err = testing::internal::GetCapturedStderr();

    ASSERT_NE(err.find("Client ID 999 not found for protocol UDP"), std::string::npos);
}

TEST(ServerTests, FindClientByIdNotFound)
{
    resetServerState();

    ClientInfo* result = server->findClientById(9999, "UDP");
    ASSERT_EQ(result, nullptr);
}

TEST(ServerTests, RegisterClientInvalidProtocol)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &mockAddr.sin_addr);

    testing::internal::CaptureStderr();
    server->registerClient(3333, "XYZ", mockAddr, 0);
    std::string err = testing::internal::GetCapturedStderr();

    ASSERT_NE(err.find("Unknown protocol: XYZ"), std::string::npos);
}

TEST(ServerTests, RegisterClientDuplicate)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &mockAddr.sin_addr);

    server->registerClient(4444, "UDP", mockAddr, 0);

    testing::internal::CaptureStderr();
    server->registerClient(4444, "UDP", mockAddr, 0); // misma PID
    std::string err = testing::internal::GetCapturedStderr();

    ASSERT_NE(err.find("Client with PID 4444 is already registered."), std::string::npos);
}

TEST(ServerTests, ForwardMessageToUdpClient)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &mockAddr.sin_addr);
    mockAddr.sin_port = htons(5000);

    int clientId = server->registerClient(1234, "UDP", mockAddr, 0);

    // Evita violación de segmento: seteás el descriptor
    int fds[2];
    pipe(fds);
    clientMapUdp[clientId].socket_fd = fds[1];

    testing::internal::CaptureStdout();
    server->forwardMessageToClient("Hola", clientId, "udp");
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_NE(output.find("Message forwarded to UDP client #" + std::to_string(clientId)), std::string::npos);
}

TEST(ServerTests, RegisterClientSuccessUDP)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &mockAddr.sin_addr);
    mockAddr.sin_port = htons(5000);

    int clientId = server->registerClient(1111, "UDP", mockAddr, 1);

    ASSERT_GE(clientId, 0);
    ASSERT_TRUE(clientMapUdp.find(1111) != clientMapUdp.end());
}

TEST(ServerTests, RegisterClientSuccessTCP)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.2", &mockAddr.sin_addr);
    mockAddr.sin_port = htons(5001);

    int clientId = server->registerClient(2222, "TCP", mockAddr, 2);

    ASSERT_GE(clientId, 0);
    ASSERT_TRUE(clientMapTcp.find(2222) != clientMapTcp.end());
}

TEST(ServerTests, RegisterClientInvalidProtocol2)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.3", &mockAddr.sin_addr);
    mockAddr.sin_port = htons(5002);

    int clientId = server->registerClient(3333, "SCTP", mockAddr, 3);

    ASSERT_EQ(clientId, -1); // protocolo desconocido
}

TEST(ServerTests, RegisterClientAlreadyRegisteredUDP)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.4", &mockAddr.sin_addr);
    mockAddr.sin_port = htons(5003);

    int first = server->registerClient(4444, "UDP", mockAddr, 4);
    int second = server->registerClient(4444, "UDP", mockAddr, 5);

    ASSERT_GE(first, 0);
    ASSERT_EQ(second, -1); // cliente ya registrado
}

TEST(ServerTests, RegisterClientMaxIdExceededUDP)
{
    resetServerState();

    struct sockaddr_in mockAddr;
    mockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &mockAddr.sin_addr);
    mockAddr.sin_port = htons(5000);

    // Registrar 9999 clientes para que el siguiente exceda el límite
    for (int i = 1; i < 9995; ++i)
    {
        int result = server->registerClient(i, "UDP", mockAddr, 0);
        ASSERT_GT(result, 0);
    }

    // Intentamos registrar el cliente 10000, que debe exceder el límite
    int result = server->registerClient(10000, "UDP", mockAddr, 0);
    ASSERT_EQ(result, -1);
}

TEST(ServerTest, StartServer_HandlesClients)
{
    resetServerState();
    std::thread serverThread([&server]() {
        server->startServer(); // corre handleUdpClients y handleTcpConnections
    });

    // Dar tiempo a que el server arranque
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    int tcpClientFd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(tcpClientFd, -1) << "Error creando socket TCP";

    struct sockaddr_in serv_addr_tcp = {};
    serv_addr_tcp.sin_family = AF_INET;
    serv_addr_tcp.sin_port = htons(PORT_SERVER_TEST); // puerto TCP real
    inet_pton(AF_INET, "127.0.0.1", &serv_addr_tcp.sin_addr);

    int res = connect(tcpClientFd, (struct sockaddr*)&serv_addr_tcp, sizeof(serv_addr_tcp));
    ASSERT_NE(res, -1) << "Error conectando al servidor TCP";

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    server->stopServer();  // Detener el servidor
    server->closeServer(); // Cerrar sockets
    serverThread.detach(); // o join si implementás una forma de salir
}

// Main function
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    server = Server::getInstance(PORT_SERVER_TEST);
    int result = RUN_ALL_TESTS();
    server->closeServer();
    return result;
}
