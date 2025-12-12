#include "test_auth_proxy.h"

// Variables globales para las pruebas
AuthProxy test_proxy;

// Configuración del entorno de prueba - se ejecuta antes de cada test
void setUp(void)
{
    // Reinicia la estructura proxy
    memset(&test_proxy, 0, sizeof(AuthProxy));
}

// Limpieza después de cada test
void tearDown(void)
{
    // Si quedó alguna sesión abierta, la cerramos
    if (test_proxy.isAuthenticated)
    {
        logout_proxy(&test_proxy);
    }
}

// Tests unitarios contra la DB real

void test_create_user(void)
{
    int user;
    user = createUser("TestUser", "password_correcto", "usuario_existente@example.com", "admin");
    TEST_ASSERT_TRUE(user);
}

// Test para autenticación exitosa - necesitarás tener este usuario en tu DB
void test_authenticate_success(void)
{
    // Usar credenciales que existan en tu base de datos
    bool result = authenticate(&test_proxy, "usuario_existente@example.com", "password_correcto");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(test_proxy.isAuthenticated);
    TEST_ASSERT_NOT_EQUAL(0, strlen(test_proxy.authenticatedUser));
    TEST_ASSERT_NOT_EQUAL(0, strlen(test_proxy.authenticatedRole));
}

// Test para contraseña incorrecta
void test_authenticate_wrong_password(void)
{
    bool result = authenticate(&test_proxy, "usuario_existente@example.com", "password_incorrecto");

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_FALSE(test_proxy.isAuthenticated);
    TEST_ASSERT_EQUAL_STRING("", test_proxy.authenticatedUser);
    TEST_ASSERT_EQUAL_STRING("", test_proxy.authenticatedRole);
}

// Test para usuario no encontrado
void test_authenticate_user_not_found(void)
{
    bool result = authenticate(&test_proxy, "usuario_no_existente@example.com", "cualquier_password");

    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_FALSE(test_proxy.isAuthenticated);
    TEST_ASSERT_EQUAL_STRING("", test_proxy.authenticatedUser);
    TEST_ASSERT_EQUAL_STRING("", test_proxy.authenticatedRole);
}

// Test para cerrar sesión con usuario autenticado
void test_logout_proxy_authenticated(void)
{
    // Primero autenticamos al usuario
    if (authenticate(&test_proxy, "usuario_existente@example.com", "password_correcto"))
    {
        // Guardamos el nombre para verificar que se borre
        char username_before[100];
        strncpy(username_before, test_proxy.authenticatedUser, sizeof(username_before));

        // Ejecutamos logout
        logout_proxy(&test_proxy);

        // Verificamos que la sesión se haya cerrado correctamente
        TEST_ASSERT_FALSE(test_proxy.isAuthenticated);
        TEST_ASSERT_EQUAL_STRING("", test_proxy.authenticatedUser);
        TEST_ASSERT_EQUAL_STRING("", test_proxy.authenticatedRole);
    }
    else
    {
        // Si no podemos autenticar, fallamos el test
        TEST_FAIL_MESSAGE("No se pudo autenticar para probar logout");
    }
}

// Test para cerrar sesión sin estar autenticado
void test_logout_proxy_not_authenticated(void)
{
    // Nos aseguramos que no hay sesión activa
    test_proxy.isAuthenticated = false;

    // Llamamos a logout
    logout_proxy(&test_proxy);

    // Verificamos que seguimos sin sesión
    TEST_ASSERT_FALSE(test_proxy.isAuthenticated);
}

// Test para manageDbUserWithAuth con usuario válido
void test_manageDbUserWithAuth_success(void)
{
    bool result = manageDbUserWithAuth(&test_proxy, "usuario_existente@example.com", "password_correcto");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(test_proxy.isAuthenticated);
}

// Test para manageDbUserWithAuth con proxy NULL
void test_manageDbUserWithAuth_null_proxy(void)
{
    bool result = manageDbUserWithAuth(NULL, "usuario_existente@example.com", "password_correcto");

    TEST_ASSERT_FALSE(result);
}

void test_delete_user(void)
{
    int delete;
    delete = deleteUser("usuario_existente@example.com");
    TEST_ASSERT_TRUE(delete);
}

// Función principal para ejecutar todos los tests
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_create_user);
    RUN_TEST(test_authenticate_success);
    RUN_TEST(test_authenticate_wrong_password);
    RUN_TEST(test_authenticate_user_not_found);
    RUN_TEST(test_logout_proxy_authenticated);
    RUN_TEST(test_logout_proxy_not_authenticated);
    RUN_TEST(test_manageDbUserWithAuth_success);
    RUN_TEST(test_manageDbUserWithAuth_null_proxy);
    RUN_TEST(test_delete_user);

    return UNITY_END();
}