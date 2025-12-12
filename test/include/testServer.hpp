/**
 * @file test_server.hpp
 * @brief Header file for testing the server's client handling functionality.
 */

#ifndef TEST_SERVER_HPP
#define TEST_SERVER_HPP

#include "server.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
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
#include <arpa/inet.h>
#include <chrono>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>

#define BUFFER_SIZE_SERVER_T 4024
#define PORT_SERVER_TEST 8080

class ServerTest : public ::testing::Test
{
  protected:
    Server* server;

    void SetUp() override;
    void TearDown() override;

    void registerClient(int socket, const std::string& protocol, const sockaddr_in& addr, int clientId);
    ClientInfo* findClientById(int clientId, const std::string& protocol);
    void cleanupInactiveUdpClients(std::chrono::seconds timeout);
    void listConnectedClients();
    void processMessage(const char* buffer, const std::string& protocol);
    void closeServer();

  private:
    int serverSocket = 0;
    int port = 8080;
};

#endif // TEST_SERVER_HPP
