#include "client.h"
#include "menu.h"

// Main function to start the application
int main()
{
    init_port(); // Initialize the port for client communication
    printf("Welcome to VAULT-TEC CLIENT !\n");

    // Start with protocol selection menu
    show_protocol_menu();

    return 0;
}
