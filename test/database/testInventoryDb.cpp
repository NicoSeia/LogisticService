#include "testInventoryDb.hpp"

void testGetWarehouseInventorySuccess()
{
    auto session = connectToDb();
    int warehouseId = 1;
    std::string product = "water";

    int result = getWarehouseInventory(session, warehouseId, product);

    TEST_ASSERT_GREATER_OR_EQUAL(0, result);
}

void testGetWarehouseInventoryFailure()
{
    auto session = connectToDb();
    int warehouseId = -1;
    std::string product = "water";

    int result = getWarehouseInventory(session, warehouseId, product);

    TEST_ASSERT_EQUAL(-1, result);
}

void testUpdateWarehouseInventorySuccess()
{
    auto session = connectToDb();
    int warehouseId = 1;
    std::string product = "water";
    int newQuantity = 50;

    int result = updateWarehouseInventory(session, warehouseId, product, newQuantity);

    TEST_ASSERT_EQUAL(1, result);
}

void testUpdateWarehouseInventoryFailure()
{
    auto session = connectToDb();
    int warehouseId = -1;
    std::string product = "water";
    int newQuantity = 50;

    int result = updateWarehouseInventory(session, warehouseId, product, newQuantity);

    TEST_ASSERT_EQUAL(-1, result);
}

void testGetHubInventorySuccess()
{
    auto session = connectToDb();
    int hubId = 2;
    std::string product = "water";

    int result = getHubInventory(session, hubId, product);

    TEST_ASSERT_GREATER_OR_EQUAL(0, result);
}

void testGetHubInventoryFailure()
{
    auto session = connectToDb();
    int hubId = -1;
    std::string product = "food";

    int result = getHubInventory(session, hubId, product);

    TEST_ASSERT_EQUAL(-1, result);
}

void testUpdateHubInventorySuccess()
{
    auto session = connectToDb();
    int hubId = 2;
    std::string product = "Water";
    int newQuantity = 30;

    int result = updateHubInventory(session, hubId, product, newQuantity);

    TEST_ASSERT_EQUAL(1, result);
}

void testUpdateHubInventoryFailure()
{
    auto session = connectToDb();
    int hubId = -1;
    std::string product = "food";
    int newQuantity = 30;

    int result = updateHubInventory(session, hubId, product, newQuantity);

    TEST_ASSERT_EQUAL(-1, result);
}

void testRealTimeUpdateSuccess()
{
    auto session = connectToDb();
    Json::Value request;
    request["general_info"]["source"]["type"] = "warehouse";
    request["general_info"]["source"]["location"] = 1;
    request["general_info"]["destination"]["type"] = "hub";
    request["general_info"]["destination"]["location"] = 1;
    request["general_info"]["action"]["product"]["name"] = "Water";
    request["general_info"]["action"]["product"]["quantity"] = 10;

    int result = realTimeUpdate(session, request);

    TEST_ASSERT_EQUAL(1, result);
}

void testRealTimeUpdateSourceFailure()
{
    auto session = connectToDb();
    Json::Value request;
    request["general_info"]["source"]["type"] = "warehouse";
    request["general_info"]["source"]["location"] = -1;
    request["general_info"]["destination"]["type"] = "hub";
    request["general_info"]["destination"]["location"] = 1;
    request["general_info"]["action"]["product"]["name"] = "Water";
    request["general_info"]["action"]["product"]["quantity"] = 180;

    int result = realTimeUpdate(session, request);

    TEST_ASSERT_EQUAL(-1, result);
}

void testRealTimeUpdateDestinationFailure()
{
    auto session = connectToDb();
    Json::Value request;
    request["general_info"]["source"]["type"] = "warehouse";
    request["general_info"]["source"]["location"] = 1;
    request["general_info"]["destination"]["type"] = "hub";
    request["general_info"]["destination"]["location"] = -1;
    request["general_info"]["action"]["product"]["name"] = "Water";
    request["general_info"]["action"]["product"]["quantity"] = 5;

    int result = realTimeUpdate(session, request);

    TEST_ASSERT_EQUAL(-1, result);
}

void testGetWarehouseInventorySQLException()
{
    auto session = connectToDb();
    int warehouseId = 99999;
    std::string product = "' OR 1=1; --";

    int result = getWarehouseInventory(session, warehouseId, product);

    TEST_ASSERT_EQUAL(-1, result);
}

void testUpdateWarehouseInventorySQLException()
{
    auto session = connectToDb();
    int warehouseId = 1;
    std::string product = "'injection";
    int quantity = 10;

    int result = updateWarehouseInventory(session, warehouseId, product, quantity);

    TEST_ASSERT_EQUAL(-1, result);
}

void testRealTimeUpdateSQLException()
{
    auto session = connectToDb();
    Json::Value request;
    request["general_info"]["source"]["type"] = "warehouse";
    request["general_info"]["source"]["location"] = 1;
    request["general_info"]["destination"]["type"] = "hub";
    request["general_info"]["destination"]["location"] = 1;
    request["general_info"]["action"]["product"]["name"] = "' OR 1=1; --";
    request["general_info"]["action"]["product"]["quantity"] = 10;

    int result = realTimeUpdate(session, request);

    TEST_ASSERT_EQUAL(-1, result);
}

void testGetWarehouseInventoryCatchPath()
{
    auto session = connectToDb();
    session.sql("USE mysql").execute();

    int result = getWarehouseInventory(session, 1, "Water");
    TEST_ASSERT_EQUAL(-1, result);
}

void testUpdateWarehouseInventoryCatchPath()
{
    auto session = connectToDb();
    session.sql("USE mysql").execute();

    int result = updateWarehouseInventory(session, 1, "Water", 10);
    TEST_ASSERT_EQUAL(-1, result);
}

void testGetHubInventoryCatchPath()
{
    auto session = connectToDb();
    session.sql("USE mysql").execute();

    int result = getHubInventory(session, 1, "Water");
    TEST_ASSERT_EQUAL(-1, result);
}

void testUpdateHubInventoryCatchPath()
{
    auto session = connectToDb();
    session.sql("USE mysql").execute();

    int result = updateHubInventory(session, 1, "Water", 10);
    TEST_ASSERT_EQUAL(-1, result);
}

void testRealTimeUpdateCatchPath()
{
    auto session = connectToDb();
    session.sql("USE mysql").execute();

    Json::Value req;
    req["general_info"]["source"]["type"] = "warehouse";
    req["general_info"]["source"]["location"] = 1;
    req["general_info"]["destination"]["type"] = "hub";
    req["general_info"]["destination"]["location"] = 1;
    req["general_info"]["action"]["product"]["name"] = "Water";
    req["general_info"]["action"]["product"]["quantity"] = 5;

    int result = realTimeUpdate(session, req);
    TEST_ASSERT_EQUAL(-1, result);
}

void testConnectToDbCatch()
{

    setenv("DB_PORT", "9999", 1);

    bool threw = false;
    try
    {
        connectToDb();
    }
    catch (const mysqlx::Error&)
    {
        threw = true;
    }

    TEST_ASSERT_TRUE_MESSAGE(threw, "connectToDb() should throw when port is invalid");

    unsetenv("DB_PORT");
}

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(testGetWarehouseInventorySuccess);
    RUN_TEST(testGetWarehouseInventoryFailure);
    RUN_TEST(testUpdateWarehouseInventorySuccess);
    RUN_TEST(testUpdateWarehouseInventoryFailure);
    RUN_TEST(testGetHubInventorySuccess);
    RUN_TEST(testGetHubInventoryFailure);
    RUN_TEST(testUpdateHubInventorySuccess);
    RUN_TEST(testUpdateHubInventoryFailure);
    RUN_TEST(testRealTimeUpdateSuccess);
    RUN_TEST(testRealTimeUpdateSourceFailure);
    RUN_TEST(testRealTimeUpdateDestinationFailure);
    RUN_TEST(testRealTimeUpdateSQLException);
    RUN_TEST(testUpdateWarehouseInventorySQLException);
    RUN_TEST(testGetWarehouseInventorySQLException);
    RUN_TEST(testGetWarehouseInventoryCatchPath);
    RUN_TEST(testUpdateWarehouseInventoryCatchPath);
    RUN_TEST(testGetHubInventoryCatchPath);
    RUN_TEST(testUpdateHubInventoryCatchPath);
    RUN_TEST(testRealTimeUpdateCatchPath);
    RUN_TEST(testConnectToDbCatch);

    return UNITY_END();
}
