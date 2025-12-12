#include "test_client.h"

FILE* original_stdin = NULL;

// -----------------------> MOCK FUNCTIONS <-----------------------------

// Restore original stdin
void restore_stdin()
{
    if (original_stdin != NULL)
    {
        fclose(stdin); // Close the redirected stdin
        stdin = original_stdin;
        original_stdin = NULL;
    }
}

void* mock_tcp_server_thread(void* arg)
{
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);
    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);

    // Response
    int response = write(new_socket, "OK", 2);
    if (response < 0)
    {
        perror("ERROR writing to socket");
    }
    else
    {
        printf("Sent: OK\n");
    }

    // close(new_socket);
    // close(server_fd);
    return NULL;
}

void* mock_udp_server_thread(void* arg)
{
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE_CLIENT];
    socklen_t addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Escucha en todas las interfaces
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR on binding UDP");
        exit(1);
    }

    recvfrom(server_sock, buffer, BUFFER_SIZE_CLIENT, 0, (struct sockaddr*)&client_addr, &addr_len);
    sendto(server_sock, "OK", 2, 0, (struct sockaddr*)&client_addr, addr_len);

    close(server_sock);
    return NULL;
}

// ---------------------> TEST FUNCTIONS <---------------------

void setUp(void)
{
}

void tearDown(void)
{
    restore_stdin();
}

void test_start_message_receiver_tcp(void)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT_TRUE(sockfd >= 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectar al servidor mock antes de lanzar el receptor
    int result = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    TEST_ASSERT_EQUAL(0, result);

    TEST_ASSERT_EQUAL(0, start_message_receiver(sockfd, &addr, "tcp"));

    // close(sockfd);
}

void test_read_json_from_file(void)
{
    cJSON* json = read_json_from_file("../config/request_format.json");
    TEST_ASSERT_NOT_NULL(json);
    cJSON_Delete(json);
}

void initialize_client_udp_test(void)
{
    struct sockaddr_in dest_addr;
    struct hostent* server;
    int sockfd;

    int result = initialize_client_udp("localhost", PORT, &sockfd, &dest_addr, &server);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_NOT_NULL(server);
    TEST_ASSERT_TRUE(sockfd > 0);
    TEST_ASSERT_EQUAL(AF_INET, dest_addr.sin_family);

    // close(sockfd);
}

void client_tcp_test(void)
{
    pthread_t server_tid;
    pthread_create(&server_tid, NULL, mock_tcp_server_thread, NULL);

    // Esperamos un poco a que el server arranque
    usleep(10000);

    const char* input_data = "USB\n"         // source.type
                             "100\n"         // source.location
                             "LAN\n"         // destination.type
                             "200\n"         // destination.location
                             "transfe\n"     // action.type
                             "P123\n"        // product.id
                             "Mouse\n"       // product.name
                             "10\n"          // product.quantity
                             "Message ABC\n" // metadata.message
                             "HIGH\n"        // metadata.priority
                             "TCP\n";        // metadata.protocol

    // Archivo temporal para simular entrada
    char tmpname[] = "/tmp/test_input_XXXXXX";
    int fd = mkstemp(tmpname);
    write(fd, input_data, strlen(input_data));
    lseek(fd, 0, SEEK_SET);

    FILE* original_stdin = stdin;
    FILE* tmp_file = fdopen(fd, "r");
    stdin = tmp_file;

    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent* server;

    int result = initialize_client_tcp("localhost", PORT, &sockfd, &server_addr, &server);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_NOT_NULL(server);
    TEST_ASSERT_TRUE(sockfd > 0);
    TEST_ASSERT_EQUAL(AF_INET, server_addr.sin_family);

    int response = client_communicate_tcp(sockfd);
    TEST_ASSERT_EQUAL(0, response);
    TEST_ASSERT_NOT_EQUAL(-1, response);
    TEST_ASSERT_NOT_EQUAL(1, response);

    // pthread_join(server_tid, NULL);
    //  Restaurar stdin
    restore_stdin();
    fclose(tmp_file);
    unlink(tmpname); // Borrar archivo temporal
    // close(sockfd);
}

void test_update_json_with_user_input_new(void)
{
    // Crear JSON base
    const char* json_str = "{"
                           "  \"general_info\": {"
                           "    \"id\": \"\","
                           "    \"source\": { \"type\": \"\", \"location\": 0 },"
                           "    \"destination\": { \"type\": \"\", \"location\": 0 },"
                           "    \"action\": {"
                           "      \"type\": \"\","
                           "      \"product\": { \"id\": \"\", \"name\": \"\", \"quantity\": 0 }"
                           "    },"
                           "    \"metadata\": {"
                           "      \"message\": \"\","
                           "      \"priority\": \"\","
                           "      \"protocol\": \"\","
                           "      \"date\": \"\""
                           "    }"
                           "  }"
                           "}";

    cJSON* json = cJSON_Parse(json_str);
    TEST_ASSERT_NOT_NULL(json);

    // Crear archivo temporal con entradas simuladas
    const char* input_data = "USB\n"         // source.type
                             "100\n"         // source.location
                             "LAN\n"         // destination.type
                             "200\n"         // destination.location
                             "transfe\n"     // action.type
                             "P123\n"        // product.id
                             "Mouse\n"       // product.name
                             "10\n"          // product.quantity
                             "Message ABC\n" // metadata.message
                             "HIGH\n"        // metadata.priority
                             "TCP\n";        // metadata.protocol

    // Crear archivo temporal
    char tmpname[] = "/tmp/test_input_XXXXXX";
    int fd = mkstemp(tmpname);
    write(fd, input_data, strlen(input_data));
    lseek(fd, 0, SEEK_SET); // Volver al inicio del archivo

    FILE* original_stdin = stdin;
    FILE* tmp_file = fdopen(fd, "r");
    stdin = tmp_file; // Redirigir stdin

    // Crear buffer y ejecutar la función
    char buffer[256];
    update_json_with_user_input(json, buffer);

    // Restaurar stdin
    stdin = original_stdin;
    fclose(tmp_file);
    unlink(tmpname); // Borrar archivo temporal

    // Verificaciones (ejemplo)
    cJSON* general_info = cJSON_GetObjectItem(json, "general_info");
    TEST_ASSERT_NOT_NULL(general_info);

    TEST_ASSERT_EQUAL_STRING("USB",
                             cJSON_GetObjectItem(cJSON_GetObjectItem(general_info, "source"), "type")->valuestring);
    TEST_ASSERT_EQUAL_INT(100, cJSON_GetObjectItem(cJSON_GetObjectItem(general_info, "source"), "location")->valueint);
    TEST_ASSERT_EQUAL_STRING(
        "LAN", cJSON_GetObjectItem(cJSON_GetObjectItem(general_info, "destination"), "type")->valuestring);

    cJSON_Delete(json);
}

void test_client_udp_real()
{
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, mock_udp_server_thread, NULL);

    usleep(10000);

    // Crear archivo temporal con entradas simuladas
    const char* input_data = "USB\n"         // source.type
                             "100\n"         // source.location
                             "LAN\n"         // destination.type
                             "200\n"         // destination.location
                             "transfe\n"     // action.type
                             "P123\n"        // product.id
                             "Mouse\n"       // product.name
                             "10\n"          // product.quantity
                             "Message ABC\n" // metadata.message
                             "HIGH\n"        // metadata.priority
                             "TCP\n";        // metadata.protocol

    // Crear archivo temporal
    char tmpname[] = "/tmp/test_input_XXXXXX";
    int fd = mkstemp(tmpname);
    write(fd, input_data, strlen(input_data));
    lseek(fd, 0, SEEK_SET); // Volver al inicio del archivo

    FILE* original_stdin = stdin;
    FILE* tmp_file = fdopen(fd, "r");
    stdin = tmp_file; // Redirigir stdin

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int result = client_communicate_udp(sockfd, &dest_addr);
    TEST_ASSERT_EQUAL(0, result);

    close(sockfd);
    pthread_join(server_thread, NULL);
}

void test_read_json_file_not_found()
{
    cJSON* json = read_json_from_file("non_existent_file.json");
    TEST_ASSERT_NULL(json);
}

void test_read_json_file_invalid_json()
{
    // Crear un archivo temporal con contenido inválido
    const char* invalid_content = "This is not JSON!";
    char tmpname[] = "/tmp/invalid_json_test_XXXXXX";
    int fd = mkstemp(tmpname);
    write(fd, invalid_content, strlen(invalid_content));
    close(fd);

    cJSON* json = read_json_from_file(tmpname);
    TEST_ASSERT_NULL(json);

    unlink(tmpname); // Limpiar archivo temporal
}

void test_read_json_file_empty_file()
{
    char tmpname[] = "/tmp/empty_json_test_XXXXXX";
    int fd = mkstemp(tmpname);
    close(fd); // Archivo vacío

    cJSON* json = read_json_from_file(tmpname);
    TEST_ASSERT_NULL(json);

    unlink(tmpname);
}

void test_initialize_client_udp_host_not_found()
{
    int sockfd;
    struct sockaddr_in dest_addr;
    struct hostent* server;

    int result = initialize_client_udp("invalid.host.name", 12345, &sockfd, &dest_addr, &server);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_initialize_client_udp_socket_error()
{
    // Simular un error de socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    TEST_ASSERT_TRUE(sockfd >= 0);

    struct sockaddr_in dest_addr;
    struct hostent* server;

    // Forzar un error al intentar enlazar el socket
    int result = bind(sockfd, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    TEST_ASSERT_EQUAL(-1, result);

    close(sockfd);
}