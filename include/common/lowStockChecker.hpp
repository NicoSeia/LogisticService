#ifndef LOW_STOCK_CHECKER_HPP
#define LOW_STOCK_CHECKER_HPP

#include "alertHandler.hpp"
#include "inventoryDb.hpp"
#include <iostream>
#include <json/json.h>
#include <mysqlx/xdevapi.h>
#include <string>

/// Maximum capacity of the warehouse for a product.
#define MAX_CAPACITY 1000
/// Threshold value for low stock alerts (20% of max stock, assumed to be 1000 units per product).
#define STOCK_THRESHOLD 200
/// Threshold value for restock alerts (10% of max stock, assumed to be 1000 units per product).
#define RESTOCK_THRESHOLD 100

/**
 * @brief Checks whether a product in a warehouse has low stock and generates an alert if necessary.
 *
 * This function inspects the order JSON to determine whether the source of the order is a warehouse.
 * If it is, it retrieves the current stock level for the specified product from the warehouse's inventory.
 * If the stock is less than or equal to STOCK_THRESHOLD (20% of max capacity), an alert message is generated.
 *
 * @param session The MySQL session used to access the inventory database.
 * @param pedidoJson The JSON object representing the order.
 * @param alertOut A reference to a string where the generated alert will be stored, if applicable.
 * @return True if a low stock alert was generated; false otherwise.
 */
bool checkLowStockAlert(mysqlx::Session& session, const Json::Value& pedidoJson, std::string& alertOut);

/**
 * @brief Checks whether a product in a warehouse needs to be re-stocked and performs the update if necessary.
 *
 * This function analyzes the provided order JSON to determine whether the source of the order is a warehouse.
 * If it is, it retrieves the current stock for the product. If the stock is less than or equal to RESTOCK_THRESHOLD
 * (10% of max capacity), it automatically replenishes the product's stock to MAX_CAPACITY using the
 * updateWarehouseInventory() method, and generates a re-stock alert message.
 *
 * @param session The MySQL session used to access the inventory database.
 * @param pedidoJson The JSON object representing the order.
 * @param alertOut A reference to a string where the generated re-stock alert will be stored, if applicable.
 * @return True if the product was re-stocked; false otherwise.
 */
bool reStock(mysqlx::Session& session, const Json::Value& pedidoJson, std::string& alertOut);

#endif // LOW_STOCK_CHECKER_HPP
