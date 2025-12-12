/**
 * @file testInventoryDb.hpp
 * @brief Header file for inventory database unit tests.
 *
 * This file declares the test functions for testing inventory database
 * functions using the Unity testing framework.
 */

#ifndef TEST_INVENTORY_DB_HPP
#define TEST_INVENTORY_DB_HPP

#include "inventoryDb.hpp"
#include "unity.h"
#include <cstring>
#include <iostream>
#include <json/json.h>
#include <mysqlx/xdevapi.h>
#include <vector>

/**
 * @brief Tests successful retrieval of warehouse inventory.
 *
 * This test checks whether the function getWarehouseInventory() correctly
 * returns a non-negative quantity for a valid warehouse ID and product.
 */
void testGetWarehouseInventorySuccess();

/**
 * @brief Tests failure case for retrieving warehouse inventory.
 *
 * This test verifies that getWarehouseInventory() returns -1 when given
 * an invalid warehouse ID.
 */
void testGetWarehouseInventoryFailure();

/**
 * @brief Tests successful update of warehouse inventory.
 *
 * This test checks whether the function updateWarehouseInventory() correctly
 * updates the inventory and returns true on success.
 */
void testUpdateWarehouseInventorySuccess();

/**
 * @brief Tests failure case for updating warehouse inventory.
 *
 * This test ensures that updateWarehouseInventory() returns false when given
 * an invalid warehouse ID.
 */
void testUpdateWarehouseInventoryFailure();

/**
 * @brief Tests successful retrieval of hub inventory.
 *
 * This test verifies that getHubInventory() correctly returns a non-negative
 * quantity for a valid hub ID and product.
 */
void testGetHubInventorySuccess();

/**
 * @brief Tests failure case for retrieving hub inventory.
 *
 * This test checks that getHubInventory() returns -1 when provided with an
 * invalid hub ID.
 */
void testGetHubInventoryFailure();

/**
 * @brief Tests successful update of hub inventory.
 *
 * This test ensures that updateHubInventory() correctly updates the inventory
 * and returns true on success.
 */
void testUpdateHubInventorySuccess();

/**
 * @brief Tests failure case for updating hub inventory.
 *
 * This test verifies that updateHubInventory() returns false when provided
 * with an invalid hub ID.
 */
void testUpdateHubInventoryFailure();

/**
 * @brief Tests successful real-time inventory update for source and
 * destination.
 *
 * This test simulates a successful real-time update request and verifies
 * that the inventories are correctly updated in both the source and
 * destination.
 */
void testRealTimeUpdateSuccess();

/**
 * @brief Tests failure case for real-time update due to invalid source
 * warehouse.
 *
 * This test verifies that the real-time update function returns failure
 * when given an invalid source warehouse.
 */
void testRealTimeUpdateSourceFailure();

/**
 * @brief Tests failure case for real-time update due to invalid destination
 * hub.
 *
 * This test ensures that the real-time update function fails when given
 * an invalid destination hub.
 */
void testRealTimeUpdateDestinationFailure();

/**
 * @brief Tests real-time update handling of SQL exceptions.
 *
 * This test simulates a failure in realTimeUpdate due to an SQL exception,
 * verifying that the exception is correctly caught and handled.
 */
void testRealTimeUpdateSQLException();

/**
 * @brief Tests warehouse inventory update handling of SQL exceptions.
 *
 * This test verifies that updateWarehouseInventory() properly handles
 * SQL exceptions and returns false on failure.
 */
void testUpdateWarehouseInventorySQLException();

/**
 * @brief Tests warehouse inventory retrieval handling of SQL exceptions.
 *
 * This test checks that getWarehouseInventory() correctly handles
 * SQL exceptions and returns -1 on failure.
 */
void testGetWarehouseInventorySQLException();

/**
 * @brief Tests failure of database connection due to invalid parameters.
 *
 * This test simulates a failed connection attempt with incorrect parameters
 * and verifies that the connection returns nullptr or an error state.
 */
void testConnectToDbFailure();

/**
 * @brief Unity setup function, called before each test.
 */
void setUp(void);

/**
 * @brief Unity teardown function, called after each test.
 */
void tearDown(void);

#endif // TEST_INVENTORY_DB_HPP
