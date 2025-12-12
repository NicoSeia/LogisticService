#ifndef TEST_CLIENT_H
#define TEST_CLIENT_H

#include "client.h"
#include "unity.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"
#include "unity_internals.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080

void* mock_server_thread(void* arg);

struct hostent* mock_gethostbyname(const char* name);

// Funciones de test unitario
void test_read_json_from_file(void);
void test_start_message_receiver_tcp(void);
void initialize_client_udp_test(void);
void client_tcp_test(void);
void test_update_json_with_user_input_new(void);
void test_client_udp_real();
void test_read_json_file_not_found();
void test_read_json_file_invalid_json();
void test_read_json_file_empty_file();

void test_initialize_client_udp_host_not_found();
void test_initialize_client_udp_socket_error();

void setUp(void);
void tearDown(void);

#endif /* TEST_CLIENT_H */
