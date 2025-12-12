/**
 * @file errorHandler.hpp
 * @brief Header file for error handler.
 *
 * This file contains declarations for the ErrorHandler class, which provides functionality to
 * generate error messages and handle exceptions. The errors are reported in JSON format,
 * with detailed information about the error or exception.
 */

#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

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
#include <exception>
#include <string>

// Define error levels

#define ERROR_CODE 500

/**
* @enum ErrorLevel
* @brief Enum representing the severity levels for errors.
*
* This enum defines the possible severity levels for error messages.
*/
enum class ErrorLevel
{
    ERROR,  /**< Represents an error message. */
    WARNING /**< Represents a warning message. */
};

/**
* @class ErrorHandler
* @brief Class responsible for handling errors and exceptions.
*
* The ErrorHandler class provides methods for generating error messages in JSON format
* and handling exceptions by converting them into JSON strings.
*/
class ErrorHandler
{
  public:
    /**
    * @brief Generates an error message in JSON format.
    *
    * This method creates a JSON string that includes error details such as the error code,
    * message, description, and the level of the error (either ERROR or WARNING).
    *
    * @param code Error code.
    * @param message Short error message.
    * @param description Detailed error description.
    * @param level Error level (default is ErrorLevel::ERROR).
    *
    * @return A JSON string containing the error details.
    */
    static std::string generateError(int code, const std::string& message, const std::string& description,
                                    ErrorLevel level = ErrorLevel::ERROR);

    /**
    * @brief Handles exceptions and converts them into a JSON message.
    *
    * This method captures exceptions and converts them into a JSON formatted string that
    * includes the exception message and the context in which the exception occurred.
    *
    * @param e The exception to be handled.
    * @param context The context where the exception occurred (optional, default is "Unknown").
    *
    * @return A JSON string containing the exception details.
    */
    static std::string handleException(const std::exception& e, const std::string& context = "Unknown");
};

#endif // ERRORHANDLER_HPP
