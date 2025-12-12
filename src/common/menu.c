#include "menu.h"
#include <string.h>

AuthProxy auth_proxy;

// ----- GLOBAL VARIABLES -----
int sockfd;
struct sockaddr_in dest_addr;
struct hostent* server;
int is_connected = 0;    // Flag to indicate if the client is connected
int is_logged_in = 0;    // Flag to indicate if user is logged in
int option_selected = 0; // Variable to store the selected option
int port;   

// ----- UTILITIES -----
void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int get_client_port() {
    char *env_port = getenv("CLIENT_PORT");
    if (env_port != NULL) {
        return atoi(env_port); // convierte el string a entero
    }
    return PORT; // valor por defecto
}

void init_port(){
    port = get_client_port();
}

int get_int_input(int min, int max)
{
    int option;
    char buffer[BUFFER_SIZE_MENU];

    while (1)
    {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            return min; // Error, return default value
        }

        // Check if input contains only one number
        if (sscanf(buffer, "%d", &option) == 1)
        {
            // Verify if there are additional characters (other than whitespace)
            char* ptr = buffer;
            while (*ptr != '\0')
            {
                if (*ptr != ' ' && *ptr != '\t' && *ptr != '\n' && (*ptr < '0' || *ptr > '9'))
                {
                    printf("Invalid input. Please enter only a number: ");
                    break;
                }
                ptr++;
            }

            if (*ptr == '\0' && option >= min && option <= max)
            {
                return option;
            }
        }

        printf("Please enter a number between %d and %d: ", min, max);
    }
}

// ----- CLIENT CONNECTION COMMANDS -----
int init_client_connection(int protocol_choice)
{
    if (protocol_choice == 1) // UDP
    {
        printf("UDP selected.\n");
        printf("Connecting to localhost:%d...\n", port);

        if (initialize_client_udp("localhost", port, &sockfd, &dest_addr, &server) != 0)
        {
            fprintf(stderr, "ERROR: Failed to initialize UDP client\n");
            return 0;
        }

        printf("UDP connection successfully established.\n");
        is_connected = 1;
        return 1;
    }
    else if (protocol_choice == 2) // TCP
    {
        printf("TCP selected.\n");
        printf("Connecting to localhost:%d...\n", port);

        if (initialize_client_tcp("localhost", port, &sockfd, &dest_addr, &server) != 0)
        {
            fprintf(stderr, "ERROR: Failed to initialize TCP client\n");
            return 0;
        }

        printf("TCP connection successfully established.\n");
        is_connected = 1;
        return 1;
    }

    return 0;
}

void start_communication(int protocol_choice)
{
    if (protocol_choice == 1) // UDP
    {
        printf("Starting UDP communication...\n");
        int result = client_communicate_udp(sockfd, &dest_addr);
        if (result == 1)
        {
            printf("UDP connection timed out.\n");
            is_connected = 0;
        }
        else if (result < 0)
        {
            printf("UDP communication error.\n");
            is_connected = 0;
        }
    }
    else if (protocol_choice == 2) // TCP
    {
        printf("Starting TCP communication...\n");
        int result = client_communicate_tcp(sockfd);
        if (result == -1)
        {
            fprintf(stderr, "ERROR: Communication failed\n");
            is_connected = 0;
        }
    }
}

// ----- AUTHENTICATION COMMANDS -----
void login()
{
    char email[BUFFER_PASS_USER], password[BUFFER_PASS_USER];

    printf("=== Login ===\n");
    printf("Email: ");
    if (fgets(email, sizeof(email), stdin) == NULL)
    {
        printf("Error reading email\n");
        return;
    }
    email[strcspn(email, "\n")] = '\0'; // Remove newline

    printf("Password: ");
    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        printf("Error reading password\n");
        return;
    }
    password[strcspn(password, "\n")] = '\0'; // Remove newline

    // Initialize authentication proxy if needed
    memset(&auth_proxy, 0, sizeof(auth_proxy));

    // Use simplified version of manageDbUserWithAuth
    if (manageDbUserWithAuth(&auth_proxy, email, password))
    {
        is_logged_in = 1;
        printf("Login successful! Welcome %s (%s)\n", auth_proxy.authenticatedUser, auth_proxy.authenticatedRole);
    }
    else
    {
        is_logged_in = 0;
        printf("Login failed. Please check your credentials.\n");
    }
}

void sign_in()
{
    printf("✅ Access granted. Proceeding to user creation.\n");
    manageDbUser();
}

void exit_program()
{
    printf("Goodbye!\n");
    exit(0);
}

// ----- PROTOCOL SELECTION COMMANDS -----
void select_protocol_and_connect()
{
    if (is_connected)
    {
        printf("An active connection already exists. Please close it before creating a new one.\n");
        return;
    }

    printf("Select protocol:\n");
    printf("1. UDP\n");
    printf("2. TCP\n");
    printf("Choose an option (1-2): ");
    fflush(stdout);

    option_selected = get_int_input(1, 2);

    if (init_client_connection(option_selected))
    {
        // Successfully connected, proceed to authentication menu
        printf("Connection established. Please authenticate.\n");
    }
}

// ----- CONNECTED CLIENT COMMANDS -----
void send_message()
{
    if (!is_connected)
    {
        printf("Error: No active connection.\n");
        return;
    }

    start_communication(option_selected);
}

void list_connected_clients()
{
    if (!is_connected)
    {
        printf("Error: No active connection.\n");
        return;
    }

    const char* msg = "LIST_CLIENTS";

    if (option_selected == 1) // UDP
    {
        ssize_t sent = sendto(sockfd, // Use the correctly initialized socket
                              msg, strlen(msg), 0,
                              (struct sockaddr*)&dest_addr, // Use the correct destination address
                              sizeof(dest_addr));

        if (sent < 0)
        {
            perror("Error sending LIST_CLIENTS via UDP");
        }
        else
        {
            printf("Sent LIST_CLIENTS request via UDP.\n");
        }
    }
    else if (option_selected == 2) // TCP
    {
        ssize_t sent = send(sockfd, // Use the connected TCP socket
                            msg, strlen(msg), 0);

        if (sent < 0)
        {
            perror("Error sending LIST_CLIENTS via TCP");
        }
        else
        {
            printf("Sent LIST_CLIENTS request via TCP.\n");
        }
    }
}

void show_message_reports()
{
    if (!is_connected)
    {
        printf("Error: No active connection.\n");
        return;
    }

    const char* msg = "SHOW_REPORT";

    if (option_selected == 1) // UDP
    {
        ssize_t sent = sendto(sockfd, // Use the correctly initialized socket
                              msg, strlen(msg), 0,
                              (struct sockaddr*)&dest_addr, // Use the correct destination address
                              sizeof(dest_addr));

        if (sent < 0)
        {
            perror("Error sending SHOW_REPORT via UDP");
        }
        else
        {
            printf("Sent SHOW_REPORT request via UDP.\n");
        }
    }
    else if (option_selected == 2) // TCP
    {
        ssize_t sent = send(sockfd, // Use the connected TCP socket
                            msg, strlen(msg), 0);

        if (sent < 0)
        {
            perror("Error sending SHOW_REPORT via TCP");
        }
        else
        {
            printf("Sent SHOW_REPORT request via TCP.\n");
        }
    }
}

void disconnect()
{
    if (is_connected)
    {
        // Close the connection
        close(sockfd);
        is_connected = 0;
        printf("Disconnected from server.\n");
        exit(0);
    }
    else
    {
        printf("No active connection to disconnect.\n");
    }
}

void logout()
{
    // Log out from authentication proxy
    logout_proxy(&auth_proxy);

    is_logged_in = 0;
    printf("Logged out successfully.\n");
}

// ----- MENU DISPLAYS -----
void show_protocol_menu();
void show_auth_menu();
void show_connected_menu();

// Protocol selection menu (First menu)
void show_protocol_menu()
{
    int option;
    do
    {
        printf("\n============= VAULT-TEC CLIENT - PROTOCOL SELECTION =============\n\n");
        printf("1. Select protocol and connect\n");
        printf("2. Exit\n");
        printf("Choose an option (1-2): ");
        fflush(stdout);

        option = get_int_input(1, 2);

        switch (option)
        {
        case 1:
            select_protocol_and_connect();
            if (is_connected)
            {
                show_auth_menu();
            }
            break;
        case 2:
            exit_program();
            break;
        default:
            printf("Invalid option. Please try again.\n");
        }
    } while (!is_connected && option != 2);
}

// Authentication menu (Second menu)
void show_auth_menu()
{
    int option;
    do
    {
        printf("\n============= AUTHENTICATION =============\n\n");
        printf("1. Login\n");
        printf("2. Sign in (Create new user)\n");
        printf("3. Disconnect\n");
        printf("Choose an option (1-3): ");
        fflush(stdout);

        option = get_int_input(1, 3);

        switch (option)
        {
        case 1:
            login();
            if (is_logged_in)
            {
                show_connected_menu();
                // If we return from connected menu but still connected,
                // we probably want to log in as a different user
                if (is_connected && !is_logged_in)
                {
                    continue;
                }
                else if (!is_connected)
                {
                    // If no longer connected, return to protocol selection
                    return;
                }
            }
            break;
        case 2:
            sign_in();
            break;
        case 3:
            disconnect();
        default:
            printf("Invalid option. Please try again.\n");
        }
    } while (is_connected && !is_logged_in && option != 3);
}

// Connected client menu (Third menu)
void show_connected_menu()
{
    int option;
    do
    {
        printf("\n============= CLIENT CONNECTED =============\n\n");
        printf("1. Send message\n");
        printf("2. View connected clients\n");
        printf("3. Show message reports\n");
        printf("4. Log out\n");
        printf("5. Disconnect\n");
        printf("Choose an option (1-5): ");
        fflush(stdout);

        option = get_int_input(1, DISCONNECT);

        switch (option)
        {
        case 1:
            send_message();
            break;
        case 2:
            list_connected_clients();
            break;
        case 3:
            show_message_reports();
            break;
        case 4:
            logout();
            if (!is_logged_in && is_connected)
            {
                // If logged out but still connected, return to auth menu
                show_auth_menu();
                return;
            }
            else if (!is_connected)
            {
                // If also disconnected, return to protocol selection
                return;
            }
            break;
        case DISCONNECT:
            disconnect();
        default:
            printf("Invalid option. Please try again.\n");
        }

        // Exit the loop if connection lost during any operation
        if (!is_connected)
        {
            printf("Connection lost. Returning to protocol selection.\n");
            return;
        }
    } while (is_connected && is_logged_in);
}
