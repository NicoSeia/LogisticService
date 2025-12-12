#include "lowStockChecker.hpp"

bool checkLowStockAlert(mysqlx::Session& session, const Json::Value& pedidoJson, std::string& alertOut)
{
    const auto& source = pedidoJson["general_info"]["source"];
    if (source["type"].asString() != "warehouse")
    {
        return false;
    }

    int warehouseId = std::stoi(source["location"].asString());
    std::string productName = pedidoJson["general_info"]["action"]["product"]["name"].asString();
    int productId = std::stoi(pedidoJson["general_info"]["action"]["product"]["id"].asString());

    int currentStock = getWarehouseInventory(session, warehouseId, productName);

    if (currentStock == -1)
    {
        std::cerr << "❌ Error trying to get warehouse inventory" << std::endl;
        return false;
    }

    if (currentStock <= STOCK_THRESHOLD)
    {
        alertOut = AlertHandler::generateAlert("Low Stock Alert: ", "Stock levels are <= 20 per cent of max capacity",
                                               productId, productName);
        return true;
    }

    return false;
}

bool reStock(mysqlx::Session& session, const Json::Value& pedidoJson, std::string& alertOut)
{
    const auto& source = pedidoJson["general_info"]["source"];
    if (source["type"].asString() != "warehouse")
    {
        return false;
    }

    int warehouseId = std::stoi(source["location"].asString());
    std::string productName = pedidoJson["general_info"]["action"]["product"]["name"].asString();
    int productId = std::stoi(pedidoJson["general_info"]["action"]["product"]["id"].asString());

    int currentStock = getWarehouseInventory(session, warehouseId, productName);

    if (currentStock == -1)
    {
        std::cerr << "❌ Error trying to get warehouse inventory" << std::endl;
        return false;
    }

    int ammountToRestock = MAX_CAPACITY - currentStock;

    if (currentStock <= RESTOCK_THRESHOLD)
    {
        int updateResult = updateWarehouseInventory(session, warehouseId, productName, ammountToRestock);

        if (updateResult == 1)
        {
            alertOut = AlertHandler::generateAlert(
                "Re-stock Alert: ", "Stock replenished to full capacity (1000 units)", productId, productName);
            return true;
        }
        else
        {
            std::cerr << "❌ Error trying to re-stock inventory" << std::endl;
            return false;
        }
    }

    return true;
}
