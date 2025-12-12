#include "testOrderValidation.hpp"
#include "orderValidation.hpp"

TEST_F(OrderValidationTest, ValidOrdersShouldPass)
{
    // Case 1: Hub requests 50 units of a non-critical product (no upper limit)
    order["general_info"]["destination"]["type"] = "hub";
    order["general_info"]["action"]["type"] = "request";
    order["general_info"]["action"]["product"]["name"] = "Meat"; // No critic
    order["general_info"]["action"]["product"]["quantity"] = 50;

    EXPECT_TRUE(validateOrderLimits(order, error)) << "Case 1 failed";

    // Case 2: Hub requests 20 units of a critical product (minimum allowed)
    order["general_info"]["action"]["product"]["name"] = "Water"; // Critic
    order["general_info"]["action"]["product"]["quantity"] = 20;

    EXPECT_TRUE(validateOrderLimits(order, error)) << "Case 2 failed";

    // Case 3: External client requests 5 units of a critical product (minimum allowed)
    order["general_info"]["destination"]["type"] = "external";
    order["general_info"]["action"]["product"]["name"] = "Medicines"; // Critic
    order["general_info"]["action"]["product"]["quantity"] = 5;

    EXPECT_TRUE(validateOrderLimits(order, error)) << "Case 3 failed";
}

TEST_F(OrderValidationTest, InvalidOrdersShouldFail)
{
    // Case 1: Hub requests less than minimum of critical product
    order["general_info"]["destination"]["type"] = "hub";
    order["general_info"]["action"]["type"] = "request";
    order["general_info"]["action"]["product"]["name"] = "Water"; // Critic
    order["general_info"]["action"]["product"]["quantity"] = 15;

    EXPECT_FALSE(validateOrderLimits(order, error)) << "Case 1 failed";

    // Case 2: External requests less than minimum of non-critical product
    order["general_info"]["destination"]["type"] = "external";
    order["general_info"]["action"]["type"] = "request";
    order["general_info"]["action"]["product"]["name"] = "Meat"; // No critic
    order["general_info"]["action"]["product"]["quantity"] = 3;

    EXPECT_FALSE(validateOrderLimits(order, error)) << "Case 2 failed";

    // Case 3: Hub requests more than maximum of critical product
    order["general_info"]["destination"]["type"] = "hub";
    order["general_info"]["action"]["type"] = "request";
    order["general_info"]["action"]["product"]["name"] = "Medicines"; // Critic
    order["general_info"]["action"]["product"]["quantity"] = 120;

    EXPECT_FALSE(validateOrderLimits(order, error)) << "Case 3 failed";

    // Case 4: External requests more than maximum of critical product
    order["general_info"]["destination"]["type"] = "external";
    order["general_info"]["action"]["type"] = "request";
    order["general_info"]["action"]["product"]["name"] = "Water"; // Critic
    order["general_info"]["action"]["product"]["quantity"] = 30;

    EXPECT_FALSE(validateOrderLimits(order, error)) << "Case 4 failed";

    // Case 5: Missing action field
    order.clear();
    order["general_info"]["destination"]["type"] = "hub";

    EXPECT_FALSE(validateOrderLimits(order, error)) << "Case 5 failed";
}

TEST_F(OrderValidationTest, MissingGeneralInfoShouldFail)
{
    order.clear();
    EXPECT_FALSE(validateOrderLimits(order, error));
    EXPECT_NE(error.find("1002"), std::string::npos);
}

TEST_F(OrderValidationTest, InvalidFieldValuesShouldFail)
{
    order["general_info"]["destination"]["type"] = ""; // Vacío
    order["general_info"]["action"]["type"] = "request";
    order["general_info"]["action"]["product"]["name"] = "";      // Vacío
    order["general_info"]["action"]["product"]["quantity"] = -10; // Inválido

    EXPECT_FALSE(validateOrderLimits(order, error));
    EXPECT_NE(error.find("1004"), std::string::npos);
}

TEST_F(OrderValidationTest, UnknownClientTypeShouldFail)
{
    order["general_info"]["destination"]["type"] = "alien";
    order["general_info"]["action"]["type"] = "request";
    order["general_info"]["action"]["product"]["name"] = "Water";
    order["general_info"]["action"]["product"]["quantity"] = 10;

    EXPECT_FALSE(validateOrderLimits(order, error));
    EXPECT_NE(error.find("1009"), std::string::npos);
}
