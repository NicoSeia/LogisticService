/**
 * @file menu.h
 * @brief Header file for the menu system, providing functions for user interaction and command handling.
 */

#ifndef MENU_H
#define MENU_H

#include "auth_proxy.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OPTIONS 5 /**< The maximum number of options in each menu. */
#define PORT 8080 /**< The port used for server communication. */
#define BUFFER_PASS_USER 110 /**< The buffer size for menu input. */
#define BUFFER_SIZE_MENU 1024 /**< The buffer size for server communication. */

#define DISCONNECT 5 /**< Constant for disconnecting from the server. */

/**
 * @struct Command
 * @brief Define el tipo para representar comandos del menú
 *
 * @details Este tipo se utiliza para manejar los diferentes comandos o acciones
 * que pueden ser ejecutados desde el menú de la aplicación
 */
typedef struct Command
{
    void (*execute)(void); /**< Function pointer to the command's execution function. */
} Command;

// ----- UTILITY FUNCTIONS -----

/**
 * @brief Clears the input buffer to prevent invalid input from previous operations.
 */
void clear_input_buffer();

/**
 * @brief Gets the port number from the environment variable or returns a default value.
 *
 * This function retrieves the port number from the CLIENT_PORT environment variable.
 * If the variable is not set, it returns a default value.
 * @return The port number for client communication.
 */
int get_client_port();

/**
 * @brief Initializes the port for client communication.
 *
 * This function sets the port number based on the environment variable or a default value.
 */
void init_port();

/**
 * @brief Gets an integer input from the user within a specified range.
 *
 * This function ensures that the user input is within the specified bounds.
 * @param min The minimum allowed value for the input.
 * @param max The maximum allowed value for the input.
 * @return The integer input within the given range.
 */
int get_int_input(int min, int max);

// ----- MENU 1 FUNCTIONS -----

/**
 * @brief Handles the login process for the user.
 *
 * Prompts the user for credentials and authenticates them.
 */
void login();

/**
 * @brief Handles the sign-in process for the user.
 *
 * Allows the user to create a new account and sign in.
 */
void sign_in();

/**
 * @brief Exits the program gracefully.
 *
 * Terminates the program, ensuring all resources are freed.
 */
void exit_program();

// ----- MENU 2 FUNCTIONS -----

/**
 * @brief Allows the user to select a protocol and establish a connection.
 *
 * Prompts the user to choose a communication protocol (e.g., TCP or UDP)
 * and attempts to connect to the server.
 */
void select_protocol_and_connect();

/**
 * @brief Navigates the user back to the main menu.
 */
void back_to_main_menu();

// ----- MENU 3 FUNCTIONS -----

/**
 * @brief Sends a message to the server.
 *
 * Allows the user to send a message after establishing a connection.
 */
void send_message();

/**
 * @brief Lists the connected clients.
 *
 * Displays the list of currently connected clients on the server.
 */
void list_connected_clients();

/**
 * @brief Displays message reports.
 *
 * Allows the user to view detailed reports of messages sent or received.
 */
void show_message_reports();

/**
 * @brief Logs the user out and terminates the session.
 *
 * Ensures that the user is logged out securely and terminates any active sessions.
 */
void logout();

// ----- MENU DISPLAY FUNCTIONS -----

/**
 * @brief Displays the main menu for the user.
 *
 * Shows the primary options available to the user (e.g., login, sign-in, etc.).
 */
void show_main_menu();

/**
 * @brief Displays the protocol selection menu.
 *
 * Allows the user to choose between available communication protocols (e.g., TCP/UDP).
 */
void show_protocol_menu();

/**
 * @brief Displays the connected clients menu.
 *
 * Allows the user to manage or interact with the list of connected clients.
 */
void show_connected_menu();

#endif // MENU_H
