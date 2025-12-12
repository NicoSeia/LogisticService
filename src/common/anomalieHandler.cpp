#include "anomalieHandler.hpp"

bool checkProductStock(const Json::Value& orderJson, std::string& errorMessage, mysqlx::Session& session)
{
    const Json::Value& generalInfo = orderJson["general_info"];
    if (generalInfo.isNull())
    {
        errorMessage =
            ErrorHandler::generateError(ERROR_INSUFFICIENT_STOCK, "Missing 'general_info' field",
                                        "The 'general_info' object is required but was not found.", ErrorLevel::ERROR);
        return false;
    }

    const Json::Value& source = generalInfo["source"];
    std::string sourceType = source["type"].asString();
    std::string sourceLocationStr = source["location"].asString();
    if (sourceType.empty() || sourceLocationStr.empty())
    {
        errorMessage = ErrorHandler::generateError(
            ERROR_INSUFFICIENT_STOCK, "Missing source type or location",
            "The 'source' object must include both 'type' and 'location' fields.", ErrorLevel::ERROR);
        return false;
    }

    int sourceId;
    try
    {
        sourceId = std::stoi(sourceLocationStr);
    }
    catch (...)
    {
        errorMessage =
            ErrorHandler::generateError(ERROR_INSUFFICIENT_STOCK, "Invalid source location format",
                                        "The source location must be a valid integer string.", ErrorLevel::ERROR);
        return false;
    }

    const Json::Value& action = generalInfo["action"];
    if (action.isNull() || !action.isMember("product"))
    {
        errorMessage = ErrorHandler::generateError(ERROR_INSUFFICIENT_STOCK, "Missing 'action' or 'product' field",
                                                   "The 'action' object and its 'product' field must exist in the "
                                                   "request.",
                                                   ErrorLevel::ERROR);
        return false;
    }

    const Json::Value& product = action["product"];
    std::string productName = product["name"].asString();
    int requestedQuantity = product["quantity"].asInt();

    if (productName.empty() || requestedQuantity <= 0)
    {
        errorMessage = ErrorHandler::generateError(
            ERROR_INSUFFICIENT_STOCK, "Invalid product data",
            "Product name must not be empty and quantity must be greater than 0.", ErrorLevel::ERROR);
        return false;
    }

    int availableStock = -1;
    if (sourceType == "hub")
    {
        availableStock = getHubInventory(session, sourceId, productName);
    }
    else if (sourceType == "warehouse")
    {
        availableStock = getWarehouseInventory(session, sourceId, productName);
    }
    else
    {
        errorMessage =
            ErrorHandler::generateError(ERROR_INSUFFICIENT_STOCK, "Unknown source type",
                                        "Source type must be either 'hub' or 'warehouse'.", ErrorLevel::ERROR);
        return false;
    }

    if (availableStock < 0)
    {
        errorMessage =
            ErrorHandler::generateError(ERROR_INSUFFICIENT_STOCK, "Inventory fetch failure",
                                        "Could not retrieve current stock from the database.", ErrorLevel::ERROR);
        return false;
    }

    if (availableStock >= requestedQuantity)
    {
        return true;
    }
    else
    {
        errorMessage = ErrorHandler::generateError(ERROR_INSUFFICIENT_STOCK, "Insufficient stock",
                                                   "Requested " + std::to_string(requestedQuantity) + ", available " +
                                                       std::to_string(availableStock),
                                                   ErrorLevel::ERROR);
        return false;
    }
}
