#include "testOrderStorage.hpp"

CoutRedirect::CoutRedirect(std::streambuf* new_buffer) : old(std::cout.rdbuf(new_buffer))
{
}

CoutRedirect::~CoutRedirect()
{
    std::cout.rdbuf(old);
}

void OrderStorageTest::SetUp()
{
    clearStoredOrders();
}

TEST_F(OrderStorageTest, StoreSingleOrder)
{
    std::string mock_json = R"({
        "general_info": {
            "id": "abc123",
            "source": { "type": "robot", "location": "zone 1" },
            "destination": { "type": "depot", "location": "zone 2" },
            "action": {
                "type": "deliver",
                "product": { "id": "prod42", "name": "oxygen", "quantity": 10 }
            },
            "metadata": {
                "date": "2025-04-06",
                "message": "delivery in progress",
                "priority": "high",
                "state": "dispatched"
            }
        }
    })";

    storeOrder(mock_json);

    std::stringstream buffer;
    {
        CoutRedirect redirect(buffer.rdbuf());
        printAllOrders();
    }

    std::string output = buffer.str();
    EXPECT_NE(output.find("Stored Orders:"), std::string::npos);
    EXPECT_NE(output.find("abc123"), std::string::npos);
    EXPECT_NE(output.find("oxygen"), std::string::npos);
}

TEST_F(OrderStorageTest, PrintAllOrders_Empty)
{
    std::stringstream buffer;
    {
        CoutRedirect redirect(buffer.rdbuf());
        printAllOrders();
    }

    std::string output = buffer.str();
    EXPECT_NE(output.find("No orders stored yet."), std::string::npos);
}

TEST_F(OrderStorageTest, ProductReportAggregation)
{
    std::string json1 = R"({
        "general_info": {
            "id": "1",
            "source": { "type": "robot", "location": "zone 1" },
            "destination": { "type": "depot", "location": "zone 2" },
            "action": {
                "type": "deliver",
                "product": { "id": "prod1", "name": "oxygen", "quantity": 5 }
            },
            "metadata": {
                "date": "2025-04-06",
                "message": "first",
                "priority": "high",
                "state": "sent"
            }
        }
    })";

    std::string json2 = R"({
        "general_info": {
            "id": "2",
            "source": { "type": "robot", "location": "zone 3" },
            "destination": { "type": "depot", "location": "zone 4" },
            "action": {
                "type": "deliver",
                "product": { "id": "prod1", "name": "oxygen", "quantity": 15 }
            },
            "metadata": {
                "date": "2025-04-07",
                "message": "second",
                "priority": "medium",
                "state": "sent"
            }
        }
    })";

    std::string json3 = R"({
        "general_info": {
            "id": "3",
            "source": { "type": "robot", "location": "zone 5" },
            "destination": { "type": "depot", "location": "zone 6" },
            "action": {
                "type": "deliver",
                "product": { "id": "prod2", "name": "water", "quantity": 20 }
            },
            "metadata": {
                "date": "2025-04-08",
                "message": "third",
                "priority": "low",
                "state": "sent"
            }
        }
    })";

    storeOrder(json1);
    storeOrder(json2);
    storeOrder(json3);

    std::stringstream buffer;
    {
        CoutRedirect redirect(buffer.rdbuf());
        printProductReport();
    }

    std::string output = buffer.str();

    EXPECT_NE(output.find("Product Quantity Report:"), std::string::npos);
    EXPECT_NE(output.find("oxygen: 20"), std::string::npos); // 5 + 15
    EXPECT_NE(output.find("water: 20"), std::string::npos);
}
