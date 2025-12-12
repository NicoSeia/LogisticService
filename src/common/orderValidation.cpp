#include "orderValidation.hpp"
#include "errorHandler.hpp"
#include <json/json.h>
#include <unordered_map>

bool validateOrderLimits(const Json::Value& root, std::string& error)
{
    const Json::Value& generalInfo = root["general_info"];
    if (generalInfo.isNull())
    {
        error = ErrorHandler::generateError(ERR_MISSING_GENERAL_INFO, "Missing general_info",
                                            "The 'general_info' field is required.",
                                            ErrorLevel::ERROR);
        return false;
    }

    const Json::Value& destination = generalInfo["destination"];
    const Json::Value& action = generalInfo["action"];
    const Json::Value& product = action["product"];

    if (destination.isNull() || action.isNull() || product.isNull())
    {
        error = ErrorHandler::generateError(ERR_MISSING_REQUIRED_FIELDS, "Missing required fields",
                                            "Fields 'destination', 'action', and 'product' are required.",
                                            ErrorLevel::ERROR);
        return false;
    }

    std::string clientType = destination["type"].asString();
    std::string actionType = action["type"].asString();
    std::string productName = product["name"].asString();
    int quantity = product["quantity"].asInt();

    if (clientType.empty() || actionType.empty() || productName.empty() || quantity <= 0)
    {
        error = ErrorHandler::generateError(ERR_INVALID_VALUES, "Invalid or missing values",
                                            "Client type, action type, product name and quantity must be valid.",
                                            ErrorLevel::ERROR);
        return false;
    }

    if (actionType != "request" && actionType != "req")
    {
        error = ErrorHandler::generateError(ERR_INVALID_ACTION_TYPE, "Invalid action type",
                                            "Action type must be 'request'.",
                                            ErrorLevel::ERROR);
        return false;
    }

    bool isCritical = criticalProducts.at(productName);

    if (clientType == "hub")
    {
        if (isCritical && (quantity < MIN_CRITICAL_HUB_QUANTITY || quantity > MAX_CRITICAL_HUB_QUANTITY))
        {
            error = ErrorHandler::generateError(ERR_INVALID_QTY_HUB_CRIT, "Invalid quantity",
                                                "Hubs must order between 20 and 100 units of critical products.",
                                                ErrorLevel::ERROR);
            return false;
        }
        else if (!isCritical && quantity < MIN_NONCRITICAL_HUB_QUANTITY)
        {
            error = ErrorHandler::generateError(ERR_INVALID_QTY_HUB_NONCRIT, "Invalid quantity",
                                                "Hubs must order at least 50 units of non-critical products.",
                                                ErrorLevel::ERROR);
            return false;
        }
    }
    else if (clientType == "external")
    {
        if (isCritical && quantity > MAX_CRITICAL_EXTERNAL_QUANTITY)
        {
            error = ErrorHandler::generateError(ERR_INVALID_QTY_EXT_CRIT, "Invalid quantity",
                                                "External clients can order up to 25 units of critical products.",
                                                ErrorLevel::ERROR);
            return false;
        }
        else if (quantity < MIN_EXTERNAL_QUANTITY)
        {
            error = ErrorHandler::generateError(ERR_INVALID_QTY_EXT_ANY, "Invalid quantity",
                                                "External clients must order at least 5 units of any product.",
                                                ErrorLevel::ERROR);
            return false;
        }
    }
    else
    {
        error = ErrorHandler::generateError(ERR_UNKNOWN_CLIENT_TYPE, "Unknown client type",
                                            "Client type must be 'hub' or 'external'.",
                                            ErrorLevel::ERROR);
        return false;
    }

    return true;
}