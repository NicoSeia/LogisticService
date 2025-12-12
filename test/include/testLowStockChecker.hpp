#ifndef TEST_LOW_STOCK_CHECKER_HPP
#define TEST_LOW_STOCK_CHECKER_HPP

#include "alertHandler.hpp"
#include "lowStockChecker.hpp"
#include <gtest/gtest.h>
#include <json/json.h>
#include <mysqlx/xdevapi.h>

/**
 * @class LowStockCheckerTest
 * @brief Test fixture for low stock alert checking logic.
 *
 * This test fixture provides a reusable JSON object representing a simulated
 * order and an alert output string, allowing derived tests to verify
 * correct alert generation behavior based on inventory levels.
 */
class LowStockCheckerTest : public ::testing::Test
{
  protected:
    Json::Value order; ///< JSON object representing a simulated order.
    std::string alert; ///< Output string containing the generated alert, if any.

    /**
     * @brief Initializes the test fixture.
     *
     * This sets up a clean base order and alert string before each test.
     */
    void SetUp() override
    {
        order.clear();
        alert.clear();
    }
};

#endif // TEST_LOW_STOCK_CHECKER_HPP
