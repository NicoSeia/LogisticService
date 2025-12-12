#include "cjson/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>

/// Maximum limit of buffer to write.
#define BUFFER_LIMIT 100
/// Buffer error or EOF.
#define BUFFER_ERROR 0

/**
 * @brief Function to get non-empty user input
 */
void get_non_empty_input(const char* prompt, char* buffer, size_t size);

/**
 * @brief Reads a JSON file and parses it into a cJSON object.
 *
 * @param filename Path to the JSON file.
 * @return Pointer to the cJSON object if successful, NULL on failure.
 */
cJSON* read_json_from_file(const char* filename);

/**
 * @brief Updates a JSON object with user input.
 *
 * This function modifies specific fields in a JSON object with values
 * entered by the user.
 *
 * @param json Pointer to the cJSON object to be updated.
 * @param buffer Buffer to store user input.
 */
void update_json_with_user_input(cJSON* json, char* buffer);