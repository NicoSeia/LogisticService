/**
 * @file auth_proxy.h
 * @brief Header file for authentication proxy.
 *
 * This file defines the `AuthProxy` structure and functions for user authentication
 * and management. It provides an interface for managing user authentication
 * through a database, including login and logout functionality.
 */

#ifndef AUTH_PROXY_H
#define AUTH_PROXY_H

#include "user_db.h"
#include <crypt.h>
#include <mysql.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROXY_BUFFER_SIZE 256
#define PROXY_MAX_EMAIL_LENGTH 100
#define PROXY_MAX_PASSWORD_LENGTH 256
#define PROXY_MAX_ROLE_LENGTH 50
#define PROXY_MAX_NAME_LENGTH 100
#define PROXY_MAX_USER_LENGTH 100

// Structure for the authentication proxy

/**
 * @struct AuthProxy
 * @brief Structure for handling user authentication state.
 *
 * This structure holds the authentication state, including information about
 * whether the user is authenticated, the authenticated user's email, and their role.
 */
typedef struct
{
    bool isAuthenticated;           /**< Flag indicating whether the user is authenticated. */
    char authenticatedUser[PROXY_MAX_USER_LENGTH];    /**< The email address of the authenticated user. */
    char authenticatedRole[PROXY_MAX_ROLE_LENGTH];    /**< The role of the authenticated user. */
} AuthProxy;

/**
 * @brief Logs out the current user.
 *
 * This function logs out the currently authenticated user by resetting the
 * `AuthProxy` structure to its default state.
 *
 * @param proxy Pointer to the AuthProxy structure that holds authentication state.
 */
void logout_proxy(AuthProxy* proxy);

/**
 * @brief Manages user authentication with the database.
 *
 * This function attempts to authenticate a user by checking their email and
 * password against the database.
 *
 * @param proxy Pointer to the AuthProxy structure to store authentication state.
 * @param email The user's email address.
 * @param password The user's password.
 *
 * @return `true` if the user is authenticated successfully, `false` otherwise.
 */
bool manageDbUserWithAuth(AuthProxy* proxy, char* email, char* password);

/**
 * @brief Authenticates a user by comparing the provided email and password.
 *
 * This function verifies whether the provided email and password match a user
 * in the database and updates the `AuthProxy` structure accordingly.
 *
 * @param proxy Pointer to the AuthProxy structure to store authentication state.
 * @param email The user's email address.
 * @param password The user's password.
 *
 * @return `true` if authentication is successful, `false` otherwise.
 */
bool authenticate(AuthProxy* proxy, const char* email, const char* password);

/**
 * @brief Establishes a connection to the database.
 *
 * This function initializes and returns a database connection using MySQL.
 *
 * @return A pointer to the MySQL connection object.
 */
MYSQL* connectToDb();

#endif /* AUTH_PROXY_H */
