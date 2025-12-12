#include "auth_proxy.h"

// Function to authenticate user
bool authenticate(AuthProxy* proxy, const char* email, const char* password)
{
    MYSQL* conn = connectToDb();
    if (!conn)
        return false;

    // Prevent SQL injection with prepared statements
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt)
    {
        fprintf(stderr, "❌ mysql_stmt_init() failed\n");
        mysql_close(conn);
        return false;
    }

    const char* query = "SELECT name, password, role FROM user WHERE email = ?";
    if (mysql_stmt_prepare(stmt, query, strlen(query)))
    {
        fprintf(stderr, "❌ mysql_stmt_prepare() failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return false;
    }

    // Bind input parameter (email)
    MYSQL_BIND bind_param[1];
    memset(bind_param, 0, sizeof(bind_param));

    unsigned long email_length = strlen(email);
    bind_param[0].buffer_type = MYSQL_TYPE_STRING;
    bind_param[0].buffer = (void*)email;
    bind_param[0].buffer_length = email_length;
    bind_param[0].length = &email_length;

    if (mysql_stmt_bind_param(stmt, bind_param))
    {
        fprintf(stderr, "❌ mysql_stmt_bind_param() failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return false;
    }

    // Execute query
    if (mysql_stmt_execute(stmt))
    {
        fprintf(stderr, "❌ mysql_stmt_execute() failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return false;
    }

    // Define result variables
    char db_name[PROXY_MAX_NAME_LENGTH];
    char db_password[PROXY_MAX_PASSWORD_LENGTH];
    char db_role[PROXY_MAX_ROLE_LENGTH];
    unsigned long name_length, password_length, role_length;
    bool is_null[3];
    bool error[3];

    // Bind output variables
    MYSQL_BIND bind_result[3];
    memset(bind_result, 0, sizeof(bind_result));

    bind_result[0].buffer_type = MYSQL_TYPE_STRING;
    bind_result[0].buffer = (void*)db_name;
    bind_result[0].buffer_length = sizeof(db_name);
    bind_result[0].length = &name_length;
    bind_result[0].is_null = &is_null[0];
    bind_result[0].error = &error[0];

    bind_result[1].buffer_type = MYSQL_TYPE_STRING;
    bind_result[1].buffer = (void*)db_password;
    bind_result[1].buffer_length = sizeof(db_password);
    bind_result[1].length = &password_length;
    bind_result[1].is_null = &is_null[1];
    bind_result[1].error = &error[1];

    bind_result[2].buffer_type = MYSQL_TYPE_STRING;
    bind_result[2].buffer = (void*)db_role;
    bind_result[2].buffer_length = sizeof(db_role);
    bind_result[2].length = &role_length;
    bind_result[2].is_null = &is_null[2];
    bind_result[2].error = &error[2];

    if (mysql_stmt_bind_result(stmt, bind_result))
    {
        fprintf(stderr, "❌ mysql_stmt_bind_result() failed: %s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return false;
    }

    // Fetch results
    bool auth_success = false;
    if (mysql_stmt_fetch(stmt) == 0)
    {
        // Verify password with crypt()
        char* salt = "$2a$12$abcdefghijklmnopqrstuvwxz0123456789";
        char* hashed = crypt(password, salt);

        // Note: in a real case, you should extract the salt from the stored hash
        // instead of using a fixed salt as in this example

        if (strcmp(hashed, db_password) == 0)
        {
            auth_success = true;
            proxy->isAuthenticated = true;
            strncpy(proxy->authenticatedUser, db_name, sizeof(proxy->authenticatedUser) - 1);
            proxy->authenticatedUser[sizeof(proxy->authenticatedUser) - 1] = '\0';

            strncpy(proxy->authenticatedRole, db_role, sizeof(proxy->authenticatedRole) - 1);
            proxy->authenticatedRole[sizeof(proxy->authenticatedRole) - 1] = '\0';

            printf("✅ Authentication successful. Welcome %s (%s)\n", proxy->authenticatedUser,
                   proxy->authenticatedRole);
        }
        else
        {
            printf("❌ Incorrect password\n");
        }
    }
    else
    {
        printf("❌ User not found\n");
    }

    mysql_stmt_close(stmt);
    mysql_close(conn);
    return auth_success;
}

void logout_proxy(AuthProxy* proxy)
{
    if (proxy->isAuthenticated)
    {
        printf("Session closed. Goodbye %s!\n", proxy->authenticatedUser);
        proxy->isAuthenticated = false;
        memset(proxy->authenticatedUser, 0, sizeof(proxy->authenticatedUser));
        memset(proxy->authenticatedRole, 0, sizeof(proxy->authenticatedRole));
    }
    else
    {
        printf("No active session.\n");
    }
}

// New simplified function to manage the system with authentication
bool manageDbUserWithAuth(AuthProxy* proxy, char* email, char* password)
{
    // Initialize the proxy if it is not already initialized
    if (proxy == NULL)
    {
        return false;
    }

    // Try to authenticate with the provided credentials
    return authenticate(proxy, email, password);
}