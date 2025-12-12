#include "orderStorage.hpp"

std::vector<std::string> storedOrders;
std::unordered_map<std::string, int> productQuantities;
std::mutex ordersMutex;

void storeOrder(const std::string& json_str)
{
    std::unique_lock<std::mutex> lock(ordersMutex);
    storedOrders.push_back(json_str);

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    Json::Value root;
    std::string errs;

    std::istringstream iss(json_str);
    if (!Json::parseFromStream(builder, iss, &root, &errs))
    {
        std::cerr << "Error parsing JSON: " << errs << "\n";
        return;
    }

    const Json::Value& general_info = root["general_info"];
    const Json::Value& action = general_info["action"];
    const Json::Value& product = action["product"];
    const std::string productName = product["name"].asString();
    const int qty = product["quantity"].asInt();

    if (!productName.empty() && qty > 0)
    {
        productQuantities[productName] += qty;
    }
}

void printAllOrders()
{
    std::lock_guard<std::mutex> lock(ordersMutex);

    if (storedOrders.empty())
    {
        std::cout << "No orders stored yet.\n";
        return;
    }

    std::cout << "Stored Orders:\n";
    for (size_t i = 0; i < storedOrders.size(); ++i)
    {
        std::cout << "[" << i + 1 << "] " << storedOrders[i] << "\n";
    }
}

void printProductReport()
{
    std::lock_guard<std::mutex> lock(ordersMutex);

    if (productQuantities.empty())
    {
        std::cout << "No product data stored yet.\n";
        return;
    }

    std::cout << "Product Quantity Report:\n";
    for (const auto& pair : productQuantities)
    {
        std::cout << "- " << pair.first << ": " << pair.second << "\n";
    }
}

void clearStoredOrders()
{
    std::lock_guard<std::mutex> lock(ordersMutex);
    storedOrders.clear();
    productQuantities.clear();
}
