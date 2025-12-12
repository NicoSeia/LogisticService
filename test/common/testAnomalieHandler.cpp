#include "testAnomalieHandler.hpp"

int getHubInventory(mysqlx::Session& session, int id, const std::string& product)
{
    if (id == 1 && product == "Water")
        return 50;
    return -1;
}

int getWarehouseInventory(mysqlx::Session& session, int id, const std::string& product)
{
    if (id == 2 && product == "Fuel")
        return 5;
    return -1;
}

// Dummy Session (no lo usamos realmente en los tests)
mysqlx::Session dummySession("127.0.0.1", 33070, "root", "root", "manage_system");

// Helper: Create a basic order JSON structure
Json::Value AnomalieHandlerTest::createOrder(const std::string& sourceType, const std::string& sourceLocation,
                                             const std::string& productName, int quantity)
{
    Json::Value order;
    order["general_info"]["source"]["type"] = sourceType;
    order["general_info"]["source"]["location"] = sourceLocation;
    order["general_info"]["action"]["product"]["name"] = productName;
    order["general_info"]["action"]["product"]["quantity"] = quantity;
    return order;
}

// Helper: Parse the error string returned from ErrorHandler
Json::Value AnomalieHandlerTest::parseErrorJson(const std::string& errorMsg)
{
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(errorMsg);
    Json::parseFromStream(builder, iss, &root, &errs);
    return root;
}

// Test: Success case with enough stock
TEST_F(AnomalieHandlerTest, SufficientStockReturnsTrue)
{
    Json::Value order = createOrder("hub", "1", "Water", 30);
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_TRUE(result);
    EXPECT_TRUE(errorMessage.empty());
}

// Test: Insufficient stock available
TEST_F(AnomalieHandlerTest, InsufficientStockReturnsFalseWithError)
{
    Json::Value order = createOrder("warehouse", "2", "Fuel", 10); // solo hay 5
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_FALSE(result);
    EXPECT_FALSE(errorMessage.empty());

    Json::Value parsedError = parseErrorJson(errorMessage);
    EXPECT_EQ(parsedError["error_code"].asInt(), ERROR_INSUFFICIENT_STOCK);
    EXPECT_EQ(parsedError["level"].asString(), "error");
    EXPECT_NE(parsedError["message"].asString().find("Insufficient stock"), std::string::npos);
}

// Test: Missing general_info field
TEST_F(AnomalieHandlerTest, MissingGeneralInfoReturnsError)
{
    Json::Value order; // general_info no se define
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_FALSE(result);
    Json::Value parsed = parseErrorJson(errorMessage);
    EXPECT_EQ(parsed["error_code"].asInt(), ERROR_INSUFFICIENT_STOCK);
    EXPECT_NE(parsed["message"].asString().find("Missing 'general_info'"), std::string::npos);
}

// Test: Missing source.type or source.location
TEST_F(AnomalieHandlerTest, MissingSourceTypeOrLocationReturnsError)
{
    Json::Value order = createOrder("", "", "Water", 10); // type y location vacíos
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_FALSE(result);
    Json::Value parsed = parseErrorJson(errorMessage);
    EXPECT_EQ(parsed["error_code"].asInt(), ERROR_INSUFFICIENT_STOCK);
    EXPECT_NE(parsed["message"].asString().find("Missing source type or location"), std::string::npos);
}

// Test: Invalid source location format (non-integer string)
TEST_F(AnomalieHandlerTest, InvalidSourceLocationFormatReturnsError)
{
    Json::Value order = createOrder("hub", "notANumber", "Water", 10);
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_FALSE(result);
    Json::Value parsed = parseErrorJson(errorMessage);
    EXPECT_EQ(parsed["error_code"].asInt(), ERROR_INSUFFICIENT_STOCK);
    EXPECT_NE(parsed["message"].asString().find("Invalid source location format"), std::string::npos);
}

// Test: Missing action or product field
TEST_F(AnomalieHandlerTest, MissingActionOrProductReturnsError)
{
    Json::Value order;
    order["general_info"]["source"]["type"] = "hub";
    order["general_info"]["source"]["location"] = "1";
    // falta action o product
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_FALSE(result);
    Json::Value parsed = parseErrorJson(errorMessage);
    EXPECT_EQ(parsed["error_code"].asInt(), ERROR_INSUFFICIENT_STOCK);
    EXPECT_NE(parsed["message"].asString().find("Missing 'action' or 'product'"), std::string::npos);
}

// Test: Product name vacío o cantidad no válida
TEST_F(AnomalieHandlerTest, InvalidProductDataReturnsError)
{
    Json::Value order = createOrder("hub", "1", "", 0); // nombre vacío y cantidad 0
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_FALSE(result);
    Json::Value parsed = parseErrorJson(errorMessage);
    EXPECT_EQ(parsed["error_code"].asInt(), ERROR_INSUFFICIENT_STOCK);
    EXPECT_NE(parsed["message"].asString().find("Invalid product data"), std::string::npos);
}

// Test: Tipo de fuente inválido
TEST_F(AnomalieHandlerTest, UnknownSourceTypeReturnsError)
{
    Json::Value order = createOrder("alienbase", "1", "Water", 10);
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_FALSE(result);
    Json::Value parsed = parseErrorJson(errorMessage);
    EXPECT_EQ(parsed["error_code"].asInt(), ERROR_INSUFFICIENT_STOCK);
    EXPECT_NE(parsed["message"].asString().find("Unknown source type"), std::string::npos);
}

// Test: Falla al obtener inventario (retorna -1)
TEST_F(AnomalieHandlerTest, InventoryFetchFailureReturnsError)
{
    Json::Value order = createOrder("hub", "99", "UnknownItem", 1); // id y producto inválidos → -1
    std::string errorMessage;
    bool result = checkProductStock(order, errorMessage, dummySession);

    EXPECT_FALSE(result);
    Json::Value parsed = parseErrorJson(errorMessage);
    EXPECT_EQ(parsed["error_code"].asInt(), ERROR_INSUFFICIENT_STOCK);
    EXPECT_NE(parsed["message"].asString().find("Inventory fetch failure"), std::string::npos);
}
