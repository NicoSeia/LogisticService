#include "server.hpp"

int main()
{
    const char* portEnv = std::getenv("SERVER_PORT");
    int port = PORT; // Valor por defecto

    if (portEnv != nullptr)
    {
        try
        {
            port = std::stoi(portEnv);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Valor inválido en SERVER_PORT. Usando puerto por defecto (8080)." << std::endl;
        }
    }

    Server* server = Server::getInstance(port);

    std::thread cleanupThread([&server]() {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(CHRONO_TIMEOUT_SECONDS));
            server->cleanupInactiveUdpClients(std::chrono::seconds(CHRONO_TIMEOUT));
        }
    });

    try
    {
        server->startServer();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error durante la ejecución del servidor: " << e.what() << std::endl;
    }

    server->closeServer();
    cleanupThread.join();

    return 0;
}
