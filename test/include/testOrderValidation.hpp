/**
 * @file testOrderValidation.hpp
 * @brief Header file for order validation tests using Google Test.
 */

#ifndef TEST_ORDER_VALIDATION_HPP
#define TEST_ORDER_VALIDATION_HPP

#include "orderValidation.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <json/json.h>

/**
 * @brief Test fixture for OrderValidation tests.
 */
class OrderValidationTest : public ::testing::Test
{
  protected:
    Json::Value order;
    Json::Value jsonOrder;
    std::string error;

    void SetUp() override
    {
        order.clear();
        jsonOrder.clear();
        error.clear();
    }

    Json::Value serializeOrder()
    {
        return order;
    }
};

#endif // TEST_ORDER_VALIDATION_HPP
