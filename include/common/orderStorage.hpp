/**
 * @file orderStorage.hpp
 * @brief Header file for order storage.
 */

#ifndef ORDER_STORAGE_H
#define ORDER_STORAGE_H

#include <iostream>
#include <json/json.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Stores all the orders as JSON-formatted strings.
 *
 * Each string represents a serialized JSON order received by the system.
 */
extern std::vector<std::string> storedOrders;

/**
 * @brief Tracks the total quantity of each product.
 *
 * The key is the product name, and the value is the cumulative quantity.
 */
extern std::unordered_map<std::string, int> productQuantities;

/**
 * @brief Mutex for synchronizing access to the orders and product data.
 *
 * Ensures thread-safe operations when storing orders or modifying quantities.
 */
extern std::mutex ordersMutex;

/**
 * @brief Stores a JSON order and updates product quantity tracking.
 *
 * Parses the input JSON string, extracts the product name and quantity,
 * stores the order, and updates internal product count data.
 *
 * @param json_str A string containing the JSON-formatted order.
 */
void storeOrder(const std::string& json_str);

/**
 * @brief Prints all stored orders to the standard output.
 *
 * Outputs each stored order JSON string with its index.
 */
void printAllOrders();

/**
 * @brief Prints a report of the total quantities of each product.
 *
 * Aggregates and displays the quantities of all products found in the stored orders.
 */
void printProductReport();

/**
 * @brief Clears all stored orders and product data.
 *
 * Resets the internal storage of orders and product quantity tracking.
 */
void clearStoredOrders();

#endif // ORDER_STORAGE_H
