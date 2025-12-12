-- Create the users table
CREATE TABLE user (
    id CHAR(36) PRIMARY KEY DEFAULT (UUID()),
    name VARCHAR(100) NOT NULL,
    password VARCHAR(255) NOT NULL,
    email VARCHAR(100) NOT NULL UNIQUE,
    role ENUM('admin', 'client') NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create the `hubs` table
DROP TABLE IF EXISTS `hubs`;
CREATE TABLE `hubs` (
  `id_hub` int DEFAULT NULL,
  `id_product` int DEFAULT NULL,
  `product_name` varchar(100) DEFAULT NULL,
  `available_quantity` int DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- Insert data into the `hubs` table
LOCK TABLES `hubs` WRITE;
INSERT INTO `hubs` VALUES (1,1,'Meat',1000),(1,2,'Water',1000),(1,3,'Medicines',1000),(1,4,'Weapons',1000),(1,5,'Clothes',1000),(2,1,'Meat',1000),(2,2,'Water',1000),(2,3,'Medicines',1000),(2,4,'Weapons',1000),(2,5,'Clothes',1000),(3,1,'Meat',1000),(3,2,'Water',1000),(3,3,'Medicines',1000),(3,4,'Weapons',1000),(3,5,'Clothes',1000);
UNLOCK TABLES;

-- Create the `warehouses` table
DROP TABLE IF EXISTS `warehouses`;
CREATE TABLE `warehouses` (
  `id_warehouse` int DEFAULT NULL,
  `id_product` int DEFAULT NULL,
  `product_name` varchar(100) DEFAULT NULL,
  `available_quantity` int DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- Insert data into the `warehouses` table
LOCK TABLES `warehouses` WRITE;
INSERT INTO `warehouses` VALUES (1,1,'Meat',1000),(1,2,'Water',1000),(1,3,'Medicines',1000),(1,4,'Weapons',1000),(1,5,'Clothes',1000),(2,1,'Meat',1000),(2,2,'Water',1000),(2,3,'Medicines',1000),(2,4,'Weapons',1000),(2,5,'Clothes',1000),(3,1,'Meat',1000),(3,2,'Water',1000),(3,3,'Medicines',1000),(3,4,'Weapons',1000),(3,5,'Clothes',1000);
UNLOCK TABLES;

-- Create stored procedures

DELIMITER ;;
CREATE PROCEDURE `updateHubInventory`(
    IN p_hub_id INT,   -- Hub ID
    IN p_product_name VARCHAR(100),    -- Product name
    IN p_quantity INT        -- Quantity to add (can be negative to subtract)
)
BEGIN
    -- Update the available quantity in the selected hub
    UPDATE hubs
    SET available_quantity = available_quantity + p_quantity
    WHERE id_hub = p_hub_id
    AND product_name = p_product_name
    AND available_quantity + p_quantity >= 0;  -- Ensure no negative quantities
END ;;
DELIMITER ;

DELIMITER ;;
CREATE PROCEDURE `updateWarehouseInventory`(
    IN p_warehouse_id INT,   -- Warehouse ID
    IN p_product_name VARCHAR(100),    -- Product name
    IN p_quantity INT        -- Quantity to add (can be negative to subtract)
)
BEGIN
    -- Update the available quantity in the selected warehouse
    UPDATE warehouses
    SET available_quantity = available_quantity + p_quantity
    WHERE id_warehouse = p_warehouse_id
    AND product_name = p_product_name
    AND available_quantity + p_quantity >= 0;  -- Ensure no negative quantities
END ;;
DELIMITER ;

DELIMITER ;;
CREATE PROCEDURE `getHubInventory`( 
	IN p_product_name  VARCHAR(100),
	IN p_hub_id INT
)
BEGIN 
	-- Get the available quantity for the product in the hub
	SELECT available_quantity 
	FROM hubs
	WHERE product_name = p_product_name AND id_hub = p_hub_id;
END ;;
DELIMITER ;

DELIMITER ;;
CREATE PROCEDURE `getWarehouseInventory`( 
	IN p_product_name  VARCHAR(100),
	IN p_warehouse_id INT
)
BEGIN 
	-- Get the available quantity for the product in the warehouse
	SELECT available_quantity 
	FROM warehouses
	WHERE product_name = p_product_name AND id_warehouse = p_warehouse_id;
END ;;
DELIMITER ;
