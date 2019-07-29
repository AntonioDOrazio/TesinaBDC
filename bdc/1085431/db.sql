CREATE USER 'test-user'@'localhost' IDENTIFIED BY 'testtest';
GRANT ALL PRIVILEGES ON * . * TO 'test-user'@'localhost';

CREATE DATABASE MySampleDB;
USE MySampleDB;

CREATE TABLE customer 
( 
	customer_id int NOT NULL AUTO_INCREMENT, 
	customer_name char(20) NOT NULL, 
	customer_address char(20) NULL, 
	PRIMARY KEY (customer_id) 
) ENGINE=InnoDB;
