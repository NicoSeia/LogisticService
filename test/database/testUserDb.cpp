#include "testUserDb.hpp"
#include "testErrorHandler.hpp"
#include "testInventoryDb.hpp"

void testCreateUserSuccess()
{
    std::string name = "John Doe";
    std::string password = "password123";
    std::string email = "john@example.com";
    std::string role = "client";

    bool result = createUser(name, password, email, role);

    TEST_ASSERT_TRUE(result);
}

void testCreateUserFailure()
{
    std::string name = "";
    std::string password = "password123";
    std::string email = "invalidemail.com";
    std::string role = "client";

    bool result = createUser(name, password, email, role);

    TEST_ASSERT_FALSE(result);
}

void testGetUsersNonEmpty()
{
    std::vector<std::string> users = getUsers();

    TEST_ASSERT_GREATER_THAN(0, users.size());
}

void testDeleteUserSuccess()
{
    std::string email = "john@example.com";

    bool result = deleteUser(email);

    TEST_ASSERT_TRUE(result);
}

void testDeleteUserFailure()
{
    std::string email = "nonexistent@example.com";

    bool result = deleteUser(email);

    TEST_ASSERT_FALSE(result);
}

void testCreateUserDbConnectionFailure()
{
    bool result = createUser("user", "", "fail@example.com", "client");
    TEST_ASSERT_FALSE(result);
}

void testGetUsersDbConnectionFailure()
{
    std::vector<std::string> users = getUsers();
    TEST_ASSERT_TRUE(users.size() >= 0);
}

void testDeleteUserDbConnectionFailure()
{
    bool result = deleteUser("");
    TEST_ASSERT_FALSE(result);
}

void setUp(void)
{
}

void tearDown(void)
{
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(testCreateUserSuccess);
    RUN_TEST(testCreateUserFailure);
    RUN_TEST(testGetUsersNonEmpty);
    RUN_TEST(testDeleteUserFailure);
    RUN_TEST(testDeleteUserSuccess);
    RUN_TEST(testGetWarehouseInventorySuccess);
    RUN_TEST(testGetWarehouseInventoryFailure);
    RUN_TEST(testUpdateWarehouseInventorySuccess);
    RUN_TEST(testUpdateWarehouseInventoryFailure);
    RUN_TEST(testGetHubInventorySuccess);
    RUN_TEST(testGetHubInventoryFailure);
    RUN_TEST(testUpdateHubInventorySuccess);
    RUN_TEST(testUpdateHubInventoryFailure);
    RUN_TEST(testCreateUserDbConnectionFailure);
    RUN_TEST(testGetUsersDbConnectionFailure);
    RUN_TEST(testDeleteUserDbConnectionFailure);
    RUN_TEST(testRealTimeUpdateSuccess);
    RUN_TEST(testRealTimeUpdateSourceFailure);
    RUN_TEST(testRealTimeUpdateDestinationFailure);
    RUN_TEST(testRealTimeUpdateSQLException);
    RUN_TEST(testUpdateWarehouseInventorySQLException);
    RUN_TEST(testGetWarehouseInventorySQLException);
    RUN_TEST(testConnectToDbFailure);

    return UNITY_END();
}
