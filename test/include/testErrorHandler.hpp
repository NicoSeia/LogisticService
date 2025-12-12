/**
 * @file errorHandler.hpp
 * @brief Header file for test error handler.
 */

#ifndef TESTERRORHANDLER_HPP
#define TESTERRORHANDLER_HPP

#include "errorHandler.hpp"
#include "unity.h"
#include "json/allocator.h"
#include "json/assertions.h"
#include "json/config.h"
#include "json/forwards.h"
#include "json/json.h"
#include "json/json_features.h"
#include "json/reader.h"
#include "json/value.h"
#include "json/version.h"
#include "json/writer.h"

/**
 * @brief Tests the generation of an error JSON object.
 *
 * This function validates that the generateError method produces
 * a well-formed JSON object containing an error code, message,
 * and additional information.
 */
void testGenerateError(void);

/**
 * @brief Tests the generation of a warning JSON object.
 *
 * This function ensures that the generateWarning method correctly
 * formats a JSON object with warning details.
 */
void testGenerateWarning(void);

/**
 * @brief Tests exception handling within the ErrorHandler.
 *
 * This function verifies that the handleException method captures
 * exceptions and returns a properly structured JSON object.
 */
void testHandleException(void);

#endif // TESTERRORHANDLER_HPP
