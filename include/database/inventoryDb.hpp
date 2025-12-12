#ifndef INVENTORY_DB_HPP
#define INVENTORY_DB_HPP

#include <iostream>
#include <json/json.h>
#include <mysqlx/xdevapi.h>
#include <string>
#include <vector>

#define PORT_DB 33070 // Default port for MySQLX

/**
 * @file inventoryDb.hpp
 * @brief Declarations for functions related to the connection to the MySQL
 * database and operations on product stock in hubs and warehouses.
 *
 * This file contains declarations of functions that allow establishing a
 * connection to the MySQL database, as well as retrieving and updating product
 * stock in hubs and warehouses.
 */

/**
 * @brief Establishes a connection to the MySQL database using MySQLX.
 *
 * @return mysqlx::Session A session object connected to the database.
 */
mysqlx::Session connectToDb();

/**
 * @brief Retrieves the available quantity of a product in a warehouse.
 *
 * @param session Active MySQL session.
 * @param warehouseId ID of the warehouse.
 * @param product Name of the product.
 * @return int Quantity available or 0/-1 on error.
 */
int getWarehouseInventory(mysqlx::Session& session, int warehouseId, const std::string& product);

/**
 * @brief Updates the available quantity of a product in a warehouse.
 *
 * @param session Active MySQL session.
 * @param warehouseId ID of the warehouse.
 * @param product Name of the product.
 * @param quantity Quantity to add/subtract.
 * @return int 1 if success, 0 on error.
 */
int updateWarehouseInventory(mysqlx::Session& session, int warehouseId, const std::string& product, int quantity);

/**
 * @brief Retrieves the available quantity of a product in a hub.
 *
 * @param session Active MySQL session.
 * @param hubId ID of the hub.
 * @param product Name of the product.
 * @return int Quantity available or 0/-1 on error.
 */
int getHubInventory(mysqlx::Session& session, int hubId, const std::string& product);

/**
 * @brief Updates the available quantity of a product in a hub.
 *
 * @param session Active MySQL session.
 * @param hubId ID of the hub.
 * @param product Name of the product.
 * @param quantity Quantity to add/subtract.
 * @return int 1 if success, 0 on error.
 */
int updateHubInventory(mysqlx::Session& session, int hubId, const std::string& product, int quantity);

/**
 * @brief Updates source and destination inventories based on a transaction.
 *
 * @param session Active MySQL session.
 * @param request JSON with transaction details.
 * @return int 1 if success, 0 on failure.
 */
int realTimeUpdate(mysqlx::Session& session, const Json::Value& request);

/**
 * @brief Displays the inventory menu.
 */
void showMenuInventory();

/**
 * @brief Handles user interaction for inventory management.
 */
void manageDbInventory();

#endif // INVENTORY_DB_HPP
