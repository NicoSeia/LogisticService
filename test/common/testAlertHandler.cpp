#include "testAlertHandler.hpp"

/**
 * @brief Test JSON alert generation.
 */
TEST(testAlertHandler, GenerateAlert)
{
    std::string jsonAlert = AlertHandler::generateAlert(
        "Inventory Mismatch", "Requested quantity exceeds available stock.", 1234, "Widget X");

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istringstream jsonStream(jsonAlert);

    ASSERT_TRUE(Json::parseFromStream(reader, jsonStream, &root, &errors));
    EXPECT_EQ(root["alert"]["name"].asString(), "Inventory Mismatch");
    EXPECT_EQ(root["alert"]["message"].asString(), "Requested quantity exceeds available stock.");
    EXPECT_EQ(root["alert"]["product_id"].asInt(), 1234);
    EXPECT_EQ(root["alert"]["product_name"].asString(), "Widget X");
}

/**
 * @brief Test empty alert handling.
 */
TEST(testAlertHandler, HandleEmptyAlert)
{
    std::string jsonAlert = AlertHandler::generateAlert("", "", 0, "");

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istringstream jsonStream(jsonAlert);

    ASSERT_TRUE(Json::parseFromStream(reader, jsonStream, &root, &errors));
    EXPECT_EQ(root["alert"]["name"].asString(), "");
    EXPECT_EQ(root["alert"]["message"].asString(), "");
    EXPECT_EQ(root["alert"]["product_id"].asInt(), 0);
    EXPECT_EQ(root["alert"]["product_name"].asString(), "");
}

/**
 * @brief Test malformed JSON handling.
 */
TEST(testAlertHandler, HandleMalformedJson)
{
    std::string malformedJson = "{alert: {name: Inventory Mismatch}}"; // Invalid JSON

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istringstream jsonStream(malformedJson);

    ASSERT_FALSE(Json::parseFromStream(reader, jsonStream, &root, &errors));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
