#include "user_db.h"

MYSQL* connectToDb()
{
    MYSQL* conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "❌ mysql_init() failed\n");
        return NULL;
    }

    // Force TCP connection instead of socket file
    const char* host = "127.0.0.1"; // Use IP address instead of "localhost"
    unsigned int port = PORT_DB;       // Your mapped port

    if (mysql_real_connect(conn, host, DB_USER, DB_PASS, DB_NAME, port, NULL, 0) == NULL)
    {
        fprintf(stderr, "❌ mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

void showMenuUser()
{
    printf("Select an option:\n");
    printf("1. Create a new user\n");
    printf("2. Get users list\n");
    printf("3. Delete a user\n");
    printf("0. End\n");
}

int getUsers()
{
    MYSQL* conn = connectToDb();
    if (!conn)
        return 0;

    if (mysql_query(conn, "SELECT name, email, role FROM user"))
    {
        fprintf(stderr, "❌ Error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res)
    {
        fprintf(stderr, "❌ mysql_store_result() failed\n");
        mysql_close(conn);
        return 0;
    }

    MYSQL_ROW row;
    int num_fields = mysql_num_fields(res);

    printf("\nUsers in database:\n");
    while ((row = mysql_fetch_row(res)))
    {
        for (int i = 0; i < num_fields; i++)
        {
            printf("%s%s", row[i] ? row[i] : "NULL", (i < num_fields - 1) ? ", " : "");
        }
        printf("\n");
    }
    printf("\n");
    mysql_free_result(res);
    mysql_close(conn);
    return 1;
}

int createUser(const char* name, const char* password, const char* email, const char* role)
{
    if (!name || !password || !email || !role || strlen(name) == 0 || strlen(password) == 0 || strlen(email) == 0 ||
        strlen(role) == 0)
    {
        fprintf(stderr, "❌ Error: All fields must have values.\n");
        return 0;
    }

    MYSQL* conn = connectToDb();
    if (!conn)
        return 0;

    char* salt = "$2a$12$abcdefghijklmnopqrstuvwxz0123456789";
    char* hashed = crypt(password, salt);

    if (!hashed)
    {
        fprintf(stderr, "❌ Error generating hash.\n");
        mysql_close(conn);
        return 0;
    }

    char query[MAX_QUERY_LENGTH];
    snprintf(query, sizeof(query), "INSERT INTO user (name, password, email, role) VALUES ('%s', '%s', '%s', '%s')",
             name, hashed, email, role);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "❌ Error creating user: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    printf("✅ User successfully created.\n");
    mysql_close(conn);
    return 1;
}

int deleteUser(const char* email)
{
    if (!email || strlen(email) == 0)
    {
        fprintf(stderr, "❌ Email cannot be empty.\n");
        return 0;
    }

    MYSQL* conn = connectToDb();
    if (!conn)
        return 0;

    char query[MAX_QUERY_LENGTH];
    snprintf(query, sizeof(query), "DELETE FROM user WHERE email='%s'", email);

    if (mysql_query(conn, query))
    {
        fprintf(stderr, "❌ Error deleting user: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    if (mysql_affected_rows(conn) > 0)
    {
        printf("✅ User successfully deleted.\n");
    }
    else
    {
        printf("❌ User not found.\n");
    }

    mysql_close(conn);
    return 1;
}

void manageDbUser()
{
    int option;
    char buffer[BUFFER_SIZE];

    while (1)
    {
        showMenuUser();
        printf("Option: ");
        if (scanf("%d", &option) != 1)
            break;
        getchar(); // clean buffer

        if (option == 1)
        {
            char name[MAX_NAME_LENGTH], password[MAX_PASSWORD_LENGTH], email[MAX_EMAIL_LENGTH], role[MAX_ROLE_LENGTH];

            printf("User name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = '\0';

            printf("Password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = '\0';

            printf("Email: ");
            fgets(email, sizeof(email), stdin);
            email[strcspn(email, "\n")] = '\0';

            printf("Select a role (admin/client): ");
            fgets(role, sizeof(role), stdin);
            role[strcspn(role, "\n")] = '\0';

            if (!createUser(name, password, email, role))
            {
                printf("❌ Failed to create user.\n");
            }
        }
        else if (option == 2)
        {
            if (!getUsers())
            {
                printf("❌ Failed to get users.\n");
            }
        }
        else if (option == 3)
        {
            char email[MAX_EMAIL_LENGTH];
            printf("Enter email to delete: ");
            fgets(email, sizeof(email), stdin);
            email[strcspn(email, "\n")] = '\0';

            if (!deleteUser(email))
            {
                printf("❌ Failed to delete user.\n");
            }
        }
        else if (option == 0)
        {
            printf("Finishing...\n");
            break;
        }
        else
        {
            printf("Invalid option. Try again.\n");
        }
    }
}
