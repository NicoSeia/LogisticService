#include "testErrorHandler.hpp"

// ✅ Test: Generating an error JSON
void testGenerateError(void)
{
    std::string jsonStr = ErrorHandler::generateError(404, "User not found", "No user registered with this email.");

    // Parse JSON
    Json::Value jsonData;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream jsonStream(jsonStr);
    Json::parseFromStream(reader, jsonStream, &jsonData, &errs);

    // Assertions
    TEST_ASSERT_EQUAL_INT(404, jsonData["error_code"].asInt());
    TEST_ASSERT_EQUAL_STRING("User not found", jsonData["message"].asCString());
    TEST_ASSERT_EQUAL_STRING("No user registered with this email.", jsonData["description"].asCString());
    TEST_ASSERT_EQUAL_STRING("error", jsonData["level"].asCString());
}

// ✅ Test: Generating a warning JSON
void testGenerateWarning(void)
{
    std::string jsonStr =
        ErrorHandler::generateError(101, "Low disk space", "Less than 10% of disk available", ErrorLevel::WARNING);

    // Parse JSON
    Json::Value jsonData;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream jsonStream(jsonStr);
    Json::parseFromStream(reader, jsonStream, &jsonData, &errs);

    // Assertions
    TEST_ASSERT_EQUAL_INT(101, jsonData["error_code"].asInt());
    TEST_ASSERT_EQUAL_STRING("Low disk space", jsonData["message"].asCString());
    TEST_ASSERT_EQUAL_STRING("Less than 10% of disk available", jsonData["description"].asCString());
    TEST_ASSERT_EQUAL_STRING("warning", jsonData["level"].asCString());
}

// ✅ Test: Handling an exception
void testHandleException(void)
{
    try
    {
        throw std::runtime_error("Test exception");
    }
    catch (const std::exception& e)
    {
        std::string jsonStr = ErrorHandler::handleException(e, "test_handleException");

        // Parse JSON
        Json::Value jsonData;
        Json::CharReaderBuilder reader;
        std::string errs;
        std::istringstream jsonStream(jsonStr);
        Json::parseFromStream(reader, jsonStream, &jsonData, &errs);

        // Assertions
        TEST_ASSERT_EQUAL_INT(500, jsonData["error_code"].asInt());
        TEST_ASSERT_EQUAL_STRING("Exception occurred", jsonData["message"].asCString());
        TEST_ASSERT_EQUAL_STRING("Test exception", jsonData["description"].asCString());
        TEST_ASSERT_EQUAL_STRING("test_handleException", jsonData["context"].asCString());
        TEST_ASSERT_EQUAL_STRING("error", jsonData["level"].asCString());
    }
}
void setUp(void)
{
    // Setup before each test (if needed)
}

void tearDown(void)
{
    // Cleanup after each test (if needed)
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(testGenerateError);
    RUN_TEST(testGenerateWarning);
    RUN_TEST(testHandleException);
    return UNITY_END();
}
