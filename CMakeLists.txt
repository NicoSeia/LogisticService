# CMake configuration file for Vault-Tec Logistics project
cmake_minimum_required(VERSION 3.15)

# Set project name and version
project(VAULT-TEC VERSION 1.0.0 LANGUAGES C CXX)

# Enable code coverage flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

include(CTest)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include/database)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/common)
include_directories(${PROJECT_SOURCE_DIR}/test/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/common/auth)
include_directories(${CONAN_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find external libraries
find_package(cJSON REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(libuuid REQUIRED)
find_package(unity REQUIRED)
find_package(GTest REQUIRED)
find_package(libxcrypt REQUIRED)
find_package(mysql-concpp REQUIRED)
find_package(libmysqlclient REQUIRED)
find_package(protobuf REQUIRED)

## -----------> Server executable
add_executable( server
                src/server/main.cpp
                src/server/server.cpp
                src/common/orderStorage.cpp
                src/common/errorHandler.cpp
                src/common/orderValidation.cpp
                src/common/anomalieHandler.cpp
                src/common/alertHandler.cpp
                src/common/lowStockChecker.cpp
                database/inventoryDb.cpp
)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/server)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/common)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/database)
target_link_libraries(server PRIVATE JsonCpp::JsonCpp mysql::concpp)

## ------------> Client executable
add_executable( client
                src/client/main.c
                src/client/client.c
                src/common/menu.c
                src/common/utils.c
                database/user_db.c
                src/common/auth/auth_proxy.c
)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/client)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/common)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/common/auth)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/database)
target_link_libraries(  client PRIVATE 
                        cjson::cjson 
                        libuuid::libuuid 
                        libmysqlclient::libmysqlclient
                        libxcrypt::libxcrypt
)

## ========== TEST CLIENT =============
add_executable( test_client
                test/client/main.c
                src/client/client.c
                test/client/test_client.c
                src/common/utils.c
)
target_include_directories(test_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/client)
target_link_libraries(test_client PRIVATE unity::unity libuuid::libuuid cjson::cjson)

## ========== TEST SERVER =============
add_executable( test_server
                test/server/testServer.cpp
                src/server/server.cpp
                src/common/orderStorage.cpp
                src/common/errorHandler.cpp
                src/common/orderValidation.cpp
                src/common/anomalieHandler.cpp
                src/common/alertHandler.cpp
                src/common/lowStockChecker.cpp
                database/inventoryDb.cpp
)
target_include_directories(test_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/server)
target_include_directories(test_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/common)
target_include_directories(test_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/database)
target_link_libraries(test_server gtest::gtest JsonCpp::JsonCpp mysql::concpp)

## ========== Test INVENTORY DB ============
add_executable( test_inventory
                test/database/testInventoryDb.cpp
                database/inventoryDb.cpp
)
target_include_directories(test_inventory PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/database)
target_link_libraries(test_inventory JsonCpp::JsonCpp mysql::concpp unity::unity)

 # Test executable for alerts, errors and anomalies
add_executable( test_alert
                test/common/testAlertHandler.cpp
                src/common/alertHandler.cpp
                test/common/testOrderValidation.cpp
                src/common/orderValidation.cpp
                src/common/errorHandler.cpp
                test/common/testAnomalieHandler.cpp
                src/common/anomalieHandler.cpp
 )
 target_include_directories(test_alert PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/database)
 target_link_libraries(test_alert JsonCpp::JsonCpp gtest::gtest mysql::concpp)

 # Test executable for low stock and re-stocks
add_executable( test_stock
                test/common/testLowStockChecker.cpp
                src/common/lowStockChecker.cpp
                src/common/alertHandler.cpp
)
target_include_directories(test_stock PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/database)
target_link_libraries(test_stock PRIVATE JsonCpp::JsonCpp gtest::gtest mysql::concpp)

# ============ Test AUTH PROXY =============
add_executable( test_auth_proxy
                test/common/auth/test_auth_proxy.c
                src/common/auth/auth_proxy.c
                database/user_db.c
)
target_include_directories(test_auth_proxy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/database)
target_include_directories(test_auth_proxy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/common)
target_include_directories(test_auth_proxy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/common/auth)
target_include_directories(test_auth_proxy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/src)
target_link_libraries(test_auth_proxy PRIVATE unity::unity libmysqlclient::libmysqlclient libxcrypt::libxcrypt)

# =========== TEST EXECUTABLE FOR ORDER STORAGE ===========
add_executable( test_order_storage
                test/common/testOrderStorage.cpp
                src/common/orderStorage.cpp
)
target_link_libraries(test_order_storage PRIVATE JsonCpp::JsonCpp gtest::gtest)

# ============================================
#           Style check target
# ============================================

find_program(CLANG_FORMAT_BIN clang-format)

if (NOT CLANG_FORMAT_BIN)
    message(FATAL_ERROR "clang-format not found! Please install clang-format.")
endif()

file(GLOB_RECURSE ALL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/include/database/*.hpp
    ${CMAKE_SOURCE_DIR}/include/database/*.h
    ${CMAKE_SOURCE_DIR}/database/*.cpp
    ${CMAKE_SOURCE_DIR}/database/*.c

    ${CMAKE_SOURCE_DIR}/include/server/*.hpp
    ${CMAKE_SOURCE_DIR}/include/client/*.h
    ${CMAKE_SOURCE_DIR}/src/server/*.cpp
    ${CMAKE_SOURCE_DIR}/src/client/*.h

    ${CMAKE_SOURCE_DIR}/include/common/*.hpp
    ${CMAKE_SOURCE_DIR}/include/common/*.h
    ${CMAKE_SOURCE_DIR}/src/common/*.cpp
    ${CMAKE_SOURCE_DIR}/src/common/*.c

    ${CMAKE_SOURCE_DIR}/include/common/auth/*.h
    ${CMAKE_SOURCE_DIR}/include/common/auth/*.hpp
    ${CMAKE_SOURCE_DIR}/src/common/auth/*.c
    ${CMAKE_SOURCE_DIR}/src/common/auth/*.cpp

    ${CMAKE_SOURCE_DIR}/test/include/*.hpp
    ${CMAKE_SOURCE_DIR}/test/include/*.h
    ${CMAKE_SOURCE_DIR}/test/client/*.c
    ${CMAKE_SOURCE_DIR}/test/server/*.cpp
    ${CMAKE_SOURCE_DIR}/test/common/*.cpp
    ${CMAKE_SOURCE_DIR}/test/common/*.c
    ${CMAKE_SOURCE_DIR}/test/database/*.cpp
    ${CMAKE_SOURCE_DIR}/test/common/auth/*.c
    ${CMAKE_SOURCE_DIR}/test/common/auth/*.cpp

)

add_custom_target(style-check
    COMMAND ${CLANG_FORMAT_BIN} --dry-run --Werror ${ALL_SOURCE_FILES}
    COMMAND ${CMAKE_COMMAND} -E echo "✅ Style check passed successfully!"
    COMMENT "Running clang-format style check..."
    VERBATIM
)

# ============================================
#           Style fix target
# ============================================

add_custom_target(style-fix
    COMMAND ${CLANG_FORMAT_BIN} -i ${ALL_SOURCE_FILES}
    COMMENT "Running clang-format to fix code style..."
    VERBATIM
)

add_custom_target(run-tests
    COMMAND ./test_client
    COMMAND ./test_server
    COMMAND ./test_inventory
    COMMAND ./test_stock
    COMMAND ./test_auth_proxy
    COMMAND ./test_alert
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS test_client test_server test_inventory test_stock test_auth_proxy test_alert
)

# Coverage target
add_custom_target(coverage
    COMMAND lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch
    COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
    COMMAND lcov --remove coverage.info '*test*' --output-file coverage.info
    #COMMAND lcov --remove coverage.info '*gtest*' --output-file coverage.info
    COMMAND lcov --remove coverage.info '*unity*' --output-file coverage.info
    COMMAND lcov --remove coverage.info '*mysqlx*' --output-file coverage.info
    COMMAND lcov --remove coverage.info '*.conan2*' --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage_out
    DEPENDS test_client test_server test_inventory test_stock test_auth_proxy test_alert
)

file(GLOB_RECURSE ALL_SOURCE_FILES_FOR_CLANG_TIDY
    ${CMAKE_SOURCE_DIR}/include/database/*.hpp
    ${CMAKE_SOURCE_DIR}/include/database/*.h
    ${CMAKE_SOURCE_DIR}/database/*.cpp
    ${CMAKE_SOURCE_DIR}/database/*.c

    ${CMAKE_SOURCE_DIR}/include/server/*.hpp
    ${CMAKE_SOURCE_DIR}/include/client/*.h
    ${CMAKE_SOURCE_DIR}/src/server/*.cpp
    ${CMAKE_SOURCE_DIR}/src/client/*.h

    ${CMAKE_SOURCE_DIR}/include/common/*.hpp
    ${CMAKE_SOURCE_DIR}/include/common/*.h
    ${CMAKE_SOURCE_DIR}/src/common/*.cpp
    ${CMAKE_SOURCE_DIR}/src/common/*.c

    ${CMAKE_SOURCE_DIR}/include/common/auth/*.h
    ${CMAKE_SOURCE_DIR}/include/common/auth/*.hpp
    ${CMAKE_SOURCE_DIR}/src/common/auth/*.c
    ${CMAKE_SOURCE_DIR}/src/common/auth/*.cpp
)

find_program(CLANG_TIDY_BIN NAMES clang-tidy)

if (NOT CLANG_TIDY_BIN)
    message(FATAL_ERROR "clang-tidy not found! Please install clang-tidy.")
endif()

add_custom_target(clang-tidy-check
    COMMAND ${CLANG_TIDY_BIN}
        -p ${CMAKE_BINARY_DIR}
        -quiet
        ${ALL_SOURCE_FILES_FOR_CLANG_TIDY}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-tidy for magic number detection..."
    VERBATIM
)
