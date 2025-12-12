#include "errorHandler.hpp"

std::string ErrorHandler::generateError(int code, const std::string& message, const std::string& description,
                                        ErrorLevel level)
{
    Json::Value errorJson;

    errorJson["error_code"] = code;
    errorJson["message"] = message;
    errorJson["description"] = description;
    errorJson["level"] = (level == ErrorLevel::ERROR) ? "error" : "warning";

    // Convert JSON object to string
    Json::StreamWriterBuilder writer;
    writer["indentation"] = ""; // Avoid line breaks
    return Json::writeString(writer, errorJson);
}

std::string ErrorHandler::handleException(const std::exception& e, const std::string& context)
{
    Json::Value errorJson;

    errorJson["error_code"] = ERROR_CODE;
    errorJson["message"] = "Exception occurred";
    errorJson["description"] = e.what();
    errorJson["context"] = context;
    errorJson["level"] = "error";

    // Convert JSON object to string
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    return Json::writeString(writer, errorJson);
}
