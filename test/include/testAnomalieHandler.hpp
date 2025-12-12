#ifndef TEST_ANOMALIE_HANDLER_HPP
#define TEST_ANOMALIE_HANDLER_HPP

#include "anomalieHandler.hpp"
#include "errorHandler.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <json/json.h>
#include <mysqlx/xdevapi.h>

/**
 * @file testAnomalieHandler.hpp
 * @brief Defines test fixture for anomaly handler testing.
 */

class AnomalieHandlerTest : public ::testing::Test
{
  protected:
    Json::Value createOrder(const std::string& sourceType, const std::string& sourceLocation,
                            const std::string& productName, int quantity);

    Json::Value parseErrorJson(const std::string& errorMsg);
};

#endif // TEST_ANOMALIE_HANDLER_HPP
