/**
 * @file testUserDb.hpp
 * @brief Header file for testing the users database handling functionality.
 */

#ifndef TESTUSERDB_HPP
#define TESTUSERDB_HPP

#include "unity.h"
#include "userDb.hpp"
#include <cstring>
#include <vector>

/**
 * @brief Tests the creation of a user in the database, expecting success.
 *
 * This test verifies that a user is correctly created in the database.
 * It ensures that the required parameters (name, password, email, role)
 * are properly handled and the user is successfully added.
 */
void testCreateUserSuccess();

/**
 * @brief Tests the creation of a user in the database, expecting failure.
 *
 * This test simulates a failure scenario when trying to create a user in the
 * database. It ensures that the system handles errors (e.g., invalid input or
 * database issues) and does not create a user in this case.
 */
void testCreateUserFailure();

/**
 * @brief Tests retrieving users from the database when the user list is
 * non-empty.
 *
 * This test checks if the system correctly retrieves a list of users when there
 * are users already in the database. It ensures that the function returns the
 * expected list of users.
 */
void testGetUsersNonEmpty();

/**
 * @brief Tests deleting a user from the database, expecting failure.
 *
 * This test simulates a failure scenario when trying to delete a user from the
 * database. It verifies that the system properly handles errors (e.g.,
 * non-existent user or other issues) and does not delete the user in this case.
 */
void testDeleteUserFailure();

/**
 * @brief Tests deleting a user from the database, expecting success.
 *
 * This test verifies that the system correctly deletes a user from the
 * database. It ensures that when a valid user is provided, the system removes
 * the user from the database.
 */
void testDeleteUserSuccess();

/**
 * @brief Tests the creation of a user when database connection fails.
 *
 * This test simulates a failure in the database connection during user creation.
 * It verifies that the function correctly handles the error and returns false.
 */
void testCreateUserDbConnectionFailure();

/**
 * @brief Tests retrieving users when database connection fails.
 *
 * This test simulates a database connection failure during the retrieval of users.
 * It checks that the function returns an empty list or handles the error gracefully.
 */
void testGetUsersDbConnectionFailure();

/**
 * @brief Tests deleting a user when database connection fails.
 *
 * This test simulates a database connection failure during the deletion of a user.
 * It verifies that the function correctly handles the error and returns false.
 */
void testDeleteUserDbConnectionFailure();

#endif // TESTUSERDB_HPP
