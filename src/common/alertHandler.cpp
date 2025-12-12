#include "alertHandler.hpp"

std::string AlertHandler::generateAlert(const std::string& name, const std::string& message, int productId,
                                        const std::string& productName)
{
    Json::Value alertJson;
    alertJson["alert"]["name"] = name;
    alertJson["alert"]["message"] = message;
    alertJson["alert"]["product_id"] = productId;
    alertJson["alert"]["product_name"] = productName;

    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, alertJson);
}
