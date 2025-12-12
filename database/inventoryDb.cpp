
#include "inventoryDb.hpp"
#include <iostream>
#include <json/json.h>
#include <string>

const int EXIT_OPTION = 0;
const int QUERY_WAREHOUSE = 1;
const int UPDATE_WAREHOUSE = 2;
const int QUERY_HUB = 3;
const int UPDATE_HUB = 4;

mysqlx::Session connectToDb()
{

    const char* env_port = std::getenv("DB_PORT");
    int port = env_port ? std::stoi(env_port) : PORT_DB;

    try
    {
        mysqlx::Session session("127.0.0.1", port, "root", "root", "manage_system");
        return session;
    }
    catch (const mysqlx::Error& err)
    {
        std::cerr << "❌ MySQL connection error: " << err.what() << std::endl;
        throw;
    }
}

int getWarehouseInventory(mysqlx::Session& session, int warehouseId, const std::string& product)
{
    try
    {
        mysqlx::SqlResult sql_result =
            session.sql("CALL getWarehouseInventory(?, ?)").bind(product, warehouseId).execute();
        mysqlx::Row row;
        if ((row = sql_result.fetchOne()))
        {
            return row[0];
        }

        return -1;
    }
    catch (const mysqlx::Error& err)
    {
        std::cerr << "❌ Error getting warehouse inventory: " << err.what() << std::endl;
        return -1;
    }
}

int updateWarehouseInventory(mysqlx::Session& session, int warehouseId, const std::string& product, int quantity)
{
    try
    {
        mysqlx::SqlResult result =
            session.sql("CALL updateWarehouseInventory(?, ?, ?)").bind(warehouseId, product, quantity).execute();

        if (result.getAffectedItemsCount() > 0)
        {
            std::cout << "✅ Warehouse inventory updated." << std::endl;
            return 1;
        }
        else
        {
            std::cerr << "❌ No warehouse records were updated." << std::endl;
            return -1;
        }
    }
    catch (const mysqlx::Error& err)
    {
        std::cerr << "❌ Error updating warehouse inventory: " << err.what() << std::endl;
        return -1;
    }
}

int getHubInventory(mysqlx::Session& session, int hubId, const std::string& product)
{
    try
    {
        mysqlx::SqlResult sql_result = session.sql("CALL getHubInventory(?, ?)").bind(product, hubId).execute();
        mysqlx::Row row;
        if ((row = sql_result.fetchOne()))
        {
            return row[0];
        }

        return -1;
    }
    catch (const mysqlx::Error& err)
    {
        std::cerr << "❌ Error getting hub inventory: " << err.what() << std::endl;
        return -1;
    }
}

int updateHubInventory(mysqlx::Session& session, int hubId, const std::string& product, int quantity)
{
    try
    {
        mysqlx::SqlResult result =
            session.sql("CALL updateHubInventory(?, ?, ?)").bind(hubId, product, quantity).execute();

        if (result.getAffectedItemsCount() > 0)
        {
            std::cout << "✅ Hub inventory updated." << std::endl;
            return 1;
        }
        else
        {
            std::cerr << "❌ No hub records were updated." << std::endl;
            return -1;
        }
    }
    catch (const mysqlx::Error& err)
    {
        std::cerr << "❌ Error updating hub inventory: " << err.what() << std::endl;
        return -1;
    }
}

int realTimeUpdate(mysqlx::Session& session, const Json::Value& request)
{
    std::string sourceType = request["general_info"]["source"]["type"].asString();
    int sourceLocation = request["general_info"]["source"]["location"].asInt();

    std::string destinationType = request["general_info"]["destination"]["type"].asString();
    int destinationLocation = request["general_info"]["destination"]["location"].asInt();

    std::string productName = request["general_info"]["action"]["product"]["name"].asString();
    int quantity = request["general_info"]["action"]["product"]["quantity"].asInt();

    int result = 0;

    if (sourceType == "hub")
    {
        result = updateHubInventory(session, sourceLocation, productName, -quantity);
    }
    else if (sourceType == "warehouse")
    {
        result = updateWarehouseInventory(session, sourceLocation, productName, -quantity);
    }

    if (result > 0)
    {
        if (destinationType == "hub")
        {
            result = updateHubInventory(session, destinationLocation, productName, quantity);
        }
        else if (destinationType == "warehouse")
        {
            result = updateWarehouseInventory(session, destinationLocation, productName, quantity);
        }
    }
    else
    {
        std::cerr << "❌ Error updating inventory in realTimeUpdate." << std::endl;
    }

    return result;
}

/*void manageDbInventory() {
  int mOption;

  while (true) {
    showMenuInventory(); // Muestra el menú
    std::cout << "Option: ";
    std::cin >> mOption;
    std::cin.ignore(); // Limpiar el buffer del cin

    if (mOption == QUERY_WAREHOUSE) {
      // Consultar inventario en un almacén
      int mWarehouseId;
      std::string mProduct;

      std::cout << "Warehouse ID: ";
      std::cin >> mWarehouseId;
      std::cin.ignore();

      std::cout << "Product name: ";
      std::getline(std::cin, mProduct);

      int mQuantity = getWarehouseInventory(mWarehouseId, mProduct);

      if (mQuantity != -1) {
        std::cout << "Stock available: " << mQuantity << std::endl;
      } else {
        std::cout << "Error retrieving inventory." << std::endl;
      }

    } else if (mOption == UPDATE_WAREHOUSE) {
      // Actualizar inventario en un almacén
      int mWarehouseId, mQuantity;
      std::string mProduct;

      std::cout << "Warehouse ID: ";
      std::cin >> mWarehouseId;
      std::cin.ignore();

      std::cout << "Product name: ";
      std::getline(std::cin, mProduct);

      std::cout << "Quantity (positive to add, negative to remove): ";
      std::cin >> mQuantity;
      std::cin.ignore();

      int mSuccess =
          updateWarehouseInventory(mWarehouseId, mProduct, mQuantity);

      if (mSuccess > 0) {
        std::cout << "Inventory successfully updated." << std::endl;
      } else {
        std::cout << "Error updating inventory." << std::endl;
      }

    } else if (mOption == QUERY_HUB) {
      // Consultar inventario en un hub
      int mHubId;
      std::string mProduct;

      std::cout << "Hub ID: ";
      std::cin >> mHubId;
      std::cin.ignore();

      std::cout << "Product name: ";
      std::getline(std::cin, mProduct);

      int mQuantity = getHubInventory(mHubId, mProduct);

      if (mQuantity != -1) {
        std::cout << "Stock available: " << mQuantity << std::endl;
      } else {
        std::cout << "Error retrieving inventory." << std::endl;
      }

    } else if (mOption == UPDATE_HUB) {
      // Actualizar inventario en un hub
      int mHubId, mQuantity;
      std::string mProduct;

      std::cout << "Hub ID: ";
      std::cin >> mHubId;
      std::cin.ignore();

      std::cout << "Product name: ";
      std::getline(std::cin, mProduct);

      std::cout << "Quantity (positive to add, negative to remove): ";
      std::cin >> mQuantity;
      std::cin.ignore();

      int mSuccess = updateHubInventory(mHubId, mProduct, mQuantity);

      if (mSuccess > 0) {
        std::cout << "Inventory successfully updated." << std::endl;
      } else {
        std::cout << "Error updating inventory." << std::endl;
      }

    } else if (mOption == EXIT_OPTION) {
      std::cout << "Finishing..." << std::endl;
      break;
    } else {
      std::cout << "Not a valid option. Try again.\n";
    }
  }
}*/
