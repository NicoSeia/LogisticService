#include "testLowStockChecker.hpp"

// Mock implementation of getWarehouseInventory
int getWarehouseInventory(mysqlx::Session& session, int warehouseId, const std::string& productName)
{
    if (warehouseId == 1 && productName == "Water")
        return 170; // Low stock
    if (warehouseId == 2 && productName == "Weapons")
        return 300; // Sufficient stock
    if (warehouseId == 3 && productName == "Water")
        return 100;
    if (warehouseId == 3 && productName == "Food")
        return -1; // Error in retrieval

    return 500; // Default stock value
}

// Mock implementation of updateWarehouseInventory
int updateWarehouseInventory(mysqlx::Session& session, int warehouseId, const std::string& productName, int newQuantity)
{
    if (warehouseId == 3 && productName == "Water" && newQuantity == 1000)
        return 1; // Emulate successful update

    if (warehouseId == 3 && productName == "Food")
        return 0; // Emulate error in update

    return 1;
}

// Dummy Session (fake one for tests)
mysqlx::Session dummySession("127.0.0.1", 33070, "root", "root", "manage_system");

TEST_F(LowStockCheckerTest, ShouldGenerateAlertWhenStockIsLow)
{
    order["general_info"]["source"]["type"] = "warehouse";
    order["general_info"]["source"]["location"] = "1";
    order["general_info"]["action"]["product"]["name"] = "Water";
    order["general_info"]["action"]["product"]["id"] = 2;

    EXPECT_TRUE(checkLowStockAlert(dummySession, order, alert));
    EXPECT_FALSE(alert.empty());
    EXPECT_NE(alert.find("Water"), std::string::npos);
}

TEST_F(LowStockCheckerTest, ShouldNotGenerateAlertWhenStockIsAboveThreshold)
{
    order["general_info"]["source"]["type"] = "warehouse";
    order["general_info"]["source"]["location"] = "2";
    order["general_info"]["action"]["product"]["name"] = "Weapons";
    order["general_info"]["action"]["product"]["id"] = 4;

    EXPECT_FALSE(checkLowStockAlert(dummySession, order, alert));
    EXPECT_TRUE(alert.empty());
}

TEST_F(LowStockCheckerTest, ShouldNotGenerateAlertIfNotWarehouse)
{
    order["general_info"]["source"]["type"] = "hub";
    order["general_info"]["source"]["location"] = "1";
    order["general_info"]["action"]["product"]["name"] = "Water";
    order["general_info"]["action"]["product"]["id"] = 2;

    EXPECT_FALSE(checkLowStockAlert(dummySession, order, alert));
    EXPECT_TRUE(alert.empty());
}

TEST_F(LowStockCheckerTest, ShouldReturnFalseIfInventoryRetrievalFails)
{
    order["general_info"]["source"]["type"] = "warehouse";
    order["general_info"]["source"]["location"] = "3";
    order["general_info"]["action"]["product"]["name"] = "Food";
    order["general_info"]["action"]["product"]["id"] = 7;

    EXPECT_FALSE(checkLowStockAlert(dummySession, order, alert));
    EXPECT_TRUE(alert.empty());
}

TEST_F(LowStockCheckerTest, ShouldRestockWhenStockIsAtOrBelowRestockThreshold)
{
    order["general_info"]["source"]["type"] = "warehouse";
    order["general_info"]["source"]["location"] = "3";
    order["general_info"]["action"]["product"]["name"] = "Water";
    order["general_info"]["action"]["product"]["id"] = 5;

    EXPECT_TRUE(reStock(dummySession, order, alert));
    EXPECT_FALSE(alert.empty());
    EXPECT_NE(alert.find("Water"), std::string::npos);
    EXPECT_NE(alert.find("Re-stock Alert: "), std::string::npos);
}

TEST_F(LowStockCheckerTest, ShouldNotRestockIfNotWarehouse)
{
    order["general_info"]["source"]["type"] = "hub";
    order["general_info"]["source"]["location"] = "3";
    order["general_info"]["action"]["product"]["name"] = "Water";
    order["general_info"]["action"]["product"]["id"] = 5;

    EXPECT_FALSE(reStock(dummySession, order, alert));
    EXPECT_TRUE(alert.empty());
}

TEST_F(LowStockCheckerTest, ShouldReturnFalseIfGetInventoryFails)
{
    order["general_info"]["source"]["type"] = "warehouse";
    order["general_info"]["source"]["location"] = "3";
    order["general_info"]["action"]["product"]["name"] = "Food";
    order["general_info"]["action"]["product"]["id"] = 7;

    EXPECT_FALSE(reStock(dummySession, order, alert));
    EXPECT_TRUE(alert.empty());
}
