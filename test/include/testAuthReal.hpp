/**
 * @file test_auth_real.hpp
 * @brief Header file for testing the real authentication server logic.
 */

#ifndef TEST_AUTH_REAL_HPP
#define TEST_AUTH_REAL_HPP

#include "authReal.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <arpa/inet.h>
#include <chrono>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

/**
 * @brief Verifies user credentials against the database.
 *
 * This function connects to the database and compares the given email and
 * password with the stored values. It uses hashing functions to check
 * the password securely.
 *
 * @param email User's email.
 * @param password User's password.
 * @return true if the credentials are correct, false otherwise.
 */
bool verifyCredentials(const std::string& email, const std::string& password);

/**
 * @brief Generates a random 6-digit session ID string.
 *
 * This function creates a unique session ID using random values to be used
 * for authenticating future requests after a successful login.
 *
 * @return A std::string containing the generated session ID.
 */
std::string generateSessionId();

/**
 * @brief Handles incoming client requests over a UDP socket.
 *
 * This function reads messages from the given socket, interprets them
 * according to the custom protocol (e.g., LOGIN or REQUEST),
 * and sends appropriate responses.
 *
 * @param sock Socket file descriptor for the UDP connection.
 */
void handleRequest(int sock);

/**
 * @brief Global session map storing active sessions.
 *
 * Maps session IDs to corresponding user emails to keep track of
 * authenticated users.
 *
 * Example:
 * - Key: "ABC123"
 * - Value: "user@example.com"
 */
extern std::unordered_map<std::string, std::string> sessions;

#endif // TEST_AUTH_REAL_HPP
