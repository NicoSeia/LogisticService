#include "testAuthReal.hpp"

class HandleRequestTest : public ::testing::Test
{
  protected:
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    void SetUp() override
    {
        server_sock = socket(AF_INET, SOCK_DGRAM, 0);
        ASSERT_NE(server_sock, -1);

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(0);

        ASSERT_EQ(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)), 0);

        socklen_t addr_len = sizeof(server_addr);
        getsockname(server_sock, (struct sockaddr*)&server_addr, &addr_len);

        client_sock = socket(AF_INET, SOCK_DGRAM, 0);
        ASSERT_NE(client_sock, -1);
    }

    void TearDown() override
    {
        close(server_sock);
        close(client_sock);
    }
};

TEST_F(HandleRequestTest, LoginAndRequestSuccessTest)
{
    std::thread server_thread_login([&]() { handleRequest(server_sock); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string login_msg = "LOGIN nicolas@seia.com nicolas";
    sendto(client_sock, login_msg.c_str(), login_msg.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    char buffer[1024] = {};
    socklen_t addr_len = sizeof(client_addr);
    ssize_t len = recvfrom(client_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);

    ASSERT_GT(len, 0);
    buffer[len] = '\0';
    EXPECT_STREQ(buffer, "OK");

    server_thread_login.join();

    ASSERT_FALSE(sessions.empty());

    std::string usuario;
    for (const auto& [id, user] : sessions)
    {
        usuario = user;
        break;
    }

    std::thread server_thread_request([&]() { handleRequest(server_sock); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string request_msg = "REQUEST " + usuario + " get_data";
    sendto(client_sock, request_msg.c_str(), request_msg.size(), 0, (struct sockaddr*)&server_addr,
           sizeof(server_addr));

    memset(buffer, 0, sizeof(buffer));
    len = recvfrom(client_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);

    ASSERT_GT(len, 0);
    buffer[len] = '\0';
    EXPECT_STREQ(buffer, "OK");

    server_thread_request.join();
}

TEST_F(HandleRequestTest, InvalidCommandReturnsError)
{
    std::thread server_thread([&]() { handleRequest(server_sock); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string invalid_msg = "FOOBAR";
    sendto(client_sock, invalid_msg.c_str(), invalid_msg.size(), 0, (struct sockaddr*)&server_addr,
           sizeof(server_addr));

    char buffer[1024] = {};
    socklen_t addr_len = sizeof(client_addr);
    ssize_t len = recvfrom(client_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);

    ASSERT_GT(len, 0);
    buffer[len] = '\0';
    EXPECT_THAT(buffer, ::testing::HasSubstr("400"));

    server_thread.join();
}

TEST_F(HandleRequestTest, LoginWithInvalidCredentialsReturnsError)
{
    std::thread server_thread([&]() { handleRequest(server_sock); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string login_msg = "LOGIN gaston@cap.com gast";
    sendto(client_sock, login_msg.c_str(), login_msg.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    char buffer[1024] = {};
    socklen_t addr_len = sizeof(client_addr);
    ssize_t len = recvfrom(client_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);

    ASSERT_GT(len, 0);
    buffer[len] = '\0';
    EXPECT_THAT(buffer, ::testing::HasSubstr("401"));

    server_thread.join();
}

TEST_F(HandleRequestTest, RequestWithoutSessionReturnsUnauthorizedError)
{
    std::thread server_thread([&]() { handleRequest(server_sock); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string request_msg = "REQUEST pepe@fail.com get_data";
    sendto(client_sock, request_msg.c_str(), request_msg.size(), 0, (struct sockaddr*)&server_addr,
           sizeof(server_addr));

    char buffer[1024] = {};
    socklen_t addr_len = sizeof(client_addr);
    ssize_t len = recvfrom(client_sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);

    ASSERT_GT(len, 0);
    buffer[len] = '\0';
    EXPECT_THAT(buffer, ::testing::HasSubstr("403"));

    server_thread.join();
}

TEST(SessionIdTest, GeneratesSixDigitId)
{
    std::string sessionId = generateSessionId();
    EXPECT_EQ(sessionId.length(), 6);
    EXPECT_TRUE(std::all_of(sessionId.begin(), sessionId.end(), ::isdigit));
}

TEST(VerifyCredentialsIntegration, ValidCredentials)
{
    EXPECT_TRUE(verifyCredentials("nicolas@seia.com", "nicolas"));
}

TEST(VerifyCredentialsIntegration, InvalidPassword)
{
    EXPECT_FALSE(verifyCredentials("gaston@cap.com", "gast"));
}

TEST(VerifyCredentialsIntegration, UserDoesNotExist)
{
    EXPECT_FALSE(verifyCredentials("nonexistent@example.com", "1234"));
}
