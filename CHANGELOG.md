## Version 1.0.1
### Release Date
[25/05/2025]

### Summary
Version 1.0.1, several improvements and fixes were made across the codebase. Magic numbers were replaced with named constants to enhance readability and maintainability. Various bugs affecting runtime behavior and logic were addressed. Additionally, variable names were updated for greater clarity and consistency, and minor refactoring was performed to clean up the code.

## Version 1.0.0
### Release Date
[29/04/2025]

### Summary
This project implements a robust client-server architecture for managing warehouse inventories, customer orders, and real-time communication across distributed users. It combines functionality such as user authentication, inventory tracking, messaging, and order validation to ensure reliable logistics and streamlined warehouse operations.

### Implemented Features
1. User login.
2. New user registration.
3. Real-time inventory update.
4. Generation of alerts by the server.
5. Detection of abnormalities in storages.
6. The server has an error handler implemented, which is used by the entire system.
7. The server knows exactly the stock of the Warehouses.
8. The server orders the restocking of the storages.
9. The server handles connections by selecting between UDP or TCP protocols to establish the connection.
10. The client must be told whether the connection was successful.
11. A new connection must not be allowed if an active connection already exists.
12. User Authentication.
    - The system must allow a user to log in with email and password.
    - The system must allow a new user to register (sign in).
    - The user must be able to log out and disconnect from the server.
13. The client can request a report of which customers are connected, so that he knows where he can place an order.
14. The client can ask the server for a report of the orders that have been placed in the last hours.
15. The server provides the client with a user interface to perform its activities.
16. The system has full control over orders, rejecting those that do not meet certain requirements, such as an excess quantity.
17. The server has the ability to delete the connection and session of inactive clients.
18. The server keeps a log of connected clients to know how to redirect messages.
19. Clients have the ability to receive messages from other clients, which are forwarded by the server.
20. Clients receive messages that their orders have been delivered.
21. In case the server shuts down at some point, the clients have the ability to handle that by retrying every so often.
22. Customers can initiate supply requests, specifying the urgency, quantity and type of resources.

### How It Was Implemented
- User Authentication: Supports secure login, registration, and logout. Only one active session per user is permitted to prevent duplication and conflicts
- Real-Time Inventory Monitoring: The system continuously updates warehouse stock levels and identifies abnormal behaviors or imbalances in storage.
- Order Management: Clients can request resources by specifying urgency and quantity. The server validates all orders, rejecting those that violate constraints (e.g., quantity limits).
- Connection Management: The server dynamically chooses between UDP or TCP for each client connection. It prevents new connections if a session is already active and provides clear success/failure feedback.
- Alerts and Messaging: The server generates alerts and forwards inter-client messages. It also notifies clients about order deliveries and maintains a log of all connected users for message routing.
- Client-Server Resilience: Clients detect server downtime and implement retry logic to re-establish connections, ensuring system robustness in unstable environments.
- Reporting Capabilities: Clients can query the server to see who is currently connected or to retrieve a log of recent orders.
- Administrative Oversight: The server can disconnect inactive users, initiate restocking procedures, and maintain a comprehensive error-handling system that spans the entire infrastructure.
