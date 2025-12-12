#include "test_client.h"

// Main function to run all tests
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_read_json_from_file);
    //RUN_TEST(client_tcp_test);
    RUN_TEST(initialize_client_udp_test);
    //RUN_TEST(test_start_message_receiver_tcp);
    RUN_TEST(test_update_json_with_user_input_new);
    RUN_TEST(test_client_udp_real);
    RUN_TEST(test_read_json_file_not_found);
    RUN_TEST(test_read_json_file_invalid_json);
    RUN_TEST(test_read_json_file_empty_file);
    RUN_TEST(test_initialize_client_udp_host_not_found);
    RUN_TEST(test_initialize_client_udp_socket_error);

    return UNITY_END();
}