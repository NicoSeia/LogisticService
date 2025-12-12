#include "utils.h"

cJSON* read_json_from_file(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("ERROR: Could not open JSON file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* json_data = (char*)malloc(file_size + 1);
    if (json_data == NULL)
    {
        perror("ERROR: Could not allocate memory for JSON data");
        fclose(file);
        return NULL;
    }

    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0'; // Null-terminate the string

    fclose(file);

    cJSON* json = cJSON_Parse(json_data);
    free(json_data);

    if (json == NULL)
    {
        fprintf(stderr, "ERROR: Could not parse JSON\n");
    }

    return json;
}

void get_non_empty_input(const char* prompt, char* buffer, size_t size)
{
    do
    {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(buffer, size, stdin) == NULL)
        {
            buffer[BUFFER_ERROR] = '\0'; // En caso de error o EOF
            break;
        }
        buffer[strcspn(buffer, "\n")] = '\0'; // Eliminar el salto de línea
    } while (strlen(buffer) == 0);            // Repetir si está vacío
}

void update_json_with_user_input(cJSON* json, char* buffer)
{

    // ---> ID
    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid, buffer);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(json, "general_info"), "id", cJSON_CreateString(buffer));

    // ---> SOURCE
    get_non_empty_input("Enter source type: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "source"), "type",
                              cJSON_CreateString(buffer));

    get_non_empty_input("Enter source location: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "source"), "location",
                              cJSON_CreateNumber(atoi(buffer)));

    // ---> DESTINATION
    get_non_empty_input("Enter destination type: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "destination"), "type",
                              cJSON_CreateString(buffer));

    get_non_empty_input("Enter destination location: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "destination"), "location",
                              cJSON_CreateNumber(atoi(buffer)));

    // ---> ACTION
    get_non_empty_input("Enter action type: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "action"), "type",
                              cJSON_CreateString(buffer));

    // ---> PRODUCT
    get_non_empty_input("Enter product id: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(
        cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "action"), "product"), "id",
        cJSON_CreateString(buffer));

    get_non_empty_input("Enter product name: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(
        cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "action"), "product"),
        "name", cJSON_CreateString(buffer));

    get_non_empty_input("Enter product quantity: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(
        cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "action"), "product"),
        "quantity", cJSON_CreateNumber(atoi(buffer)));

    // ---> METADATA
    get_non_empty_input("Enter message: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "metadata"), "message",
                              cJSON_CreateString(buffer));

    get_non_empty_input("Enter priority: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "metadata"), "priority",
                              cJSON_CreateString(buffer));

    get_non_empty_input("Enter protocol: ", buffer, BUFFER_LIMIT);
    cJSON_ReplaceItemInObject(cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "metadata"), "protocol",
                              cJSON_CreateString(buffer));

    // ---> DATE
    time_t t;
    struct tm* tm_info;
    time(&t);
    tm_info = localtime(&t);
    strftime(buffer, BUFFER_LIMIT, "%d-%m-%Y", tm_info);
    cJSON* metadata = cJSON_GetObjectItem(cJSON_GetObjectItem(json, "general_info"), "metadata");
    cJSON_ReplaceItemInObject(metadata, "date", cJSON_CreateString(buffer));
}