/**
 * @file errorHandler.hpp
 * @brief Declaration of the AlertHandler class for generating alert messages in JSON format.
 */

#ifndef ALERTHANDLER_HPP
#define ALERTHANDLER_HPP

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
#include <string>

/**
* @class AlertHandler
* @brief Handles the creation of alert messages formatted in JSON.
*
* The AlertHandler class provides a static method to generate JSON-formatted alert messages
* based on provided details such as alert type, description, product ID, and product name.
*/
class AlertHandler
{
  public:
    /**
    * @brief Default constructor for AlertHandler.
    */
    AlertHandler() = default;

    /**
    * @brief Generates an alert message in JSON format.
    *
    * This static method creates a JSON object containing alert information
    * and returns it as a string. The alert includes a name, a detailed message,
    * the related product's ID, and the related product's name.
    *
    * @param name The type or category of the alert.
    * @param message A detailed description of the issue triggering the alert.
    * @param productId The ID of the product related to the alert.
    * @param productName The name of the product related to the alert.
    * @return A string containing the alert in JSON format.
    */
    static std::string generateAlert(const std::string& name, const std::string& message, int productId,
                                    const std::string& productName);
};

#endif // ALERTHANDLER_HPP
