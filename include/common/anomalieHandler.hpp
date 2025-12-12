#ifndef ANOMALIE_HANDLER_HPP
#define ANOMALIE_HANDLER_HPP

#include "errorHandler.hpp"
#include "inventoryDb.hpp"
#include <json/json.h>
#include <mysqlx/xdevapi.h>
#include <string>

/**
 * @file anomalieHandler.hpp
 * @brief Provides functionality to detect inventory anomalies.
 *
 * This module checks if requested inventory quantities exceed the available
 * stock and generates appropriate error messages if necessary.
 * It does not send errors or alerts directly; instead, it returns the
 * validation result so that the server can handle responses accordingly.
 */

#define ERROR_INSUFFICIENT_STOCK 101 ///< Error code for insufficient stock

/**
 * @brief Checks if the requested quantity is available in inventory.
 *
 * This function reads the order JSON, retrieves the source type and location,
 * looks up the current inventory for the specified product, and determines
 * whether the requested quantity can be fulfilled.
 *
 * @param orderJson JSON object containing the full order information.
 * @param errorMessage Reference to a string where the error message will be
 * stored if stock is insufficient or malformed.
 * @param session Active MySQL session.
 * @return true if sufficient stock is available, false otherwise.
 */
bool checkProductStock(const Json::Value& orderJson, std::string& errorMessage, mysqlx::Session& session);

#endif // ANOMALIE_HANDLER_HPP
