#ifndef ORDER_VALIDATION_HPP
#define ORDER_VALIDATION_HPP

#include <json/json.h>
#include <string>
#include <unordered_map>

#define MIN_CRITICAL_HUB_QUANTITY       20
#define MAX_CRITICAL_HUB_QUANTITY       100
#define MIN_NONCRITICAL_HUB_QUANTITY    50
#define MAX_CRITICAL_EXTERNAL_QUANTITY  25
#define MIN_EXTERNAL_QUANTITY           5

#define ERR_MISSING_GENERAL_INFO     1001
#define ERR_MISSING_REQUIRED_FIELDS  1002
#define ERR_INVALID_VALUES           1003
#define ERR_INVALID_ACTION_TYPE      1004  
#define ERR_INVALID_QTY_HUB_CRIT     1005
#define ERR_INVALID_QTY_HUB_NONCRIT  1006
#define ERR_INVALID_QTY_EXT_CRIT     1007
#define ERR_INVALID_QTY_EXT_ANY      1008
#define ERR_UNKNOWN_CLIENT_TYPE      1009

/**
 * @brief A map of critical products and their status.
 *
 */
inline const std::unordered_map<std::string, bool> criticalProducts = {
    {"Water", true}, {"Medicines", true}, {"Meat", false}, {"Weapons", false}, {"Clothes", false}};

/**
 * @brief Validates the quantity limits per product order, based on client type
 * and whether the product is critical.
 *
 * This function checks if the order meets the established minimum and maximum
 * constraints for hubs and external clients, taking into account whether the
 * requested product is critical or not. It also validates the existence and
 * correct format of the required fields. In case of an error, a JSON-formatted
 * message describing the issue will be returned.
 *
 * @param orderJson A Json::Value object containing the parsed order.
 * @param error A reference to a string where a JSON-formatted error message
 * will be stored, if any.
 * @return true if the order is valid and meets all constraints; false
 * otherwise.
 */
bool validateOrderLimits(const Json::Value& orderJson, std::string& error);

#endif // ORDER_VALIDATION_HPP
