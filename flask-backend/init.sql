-- Create the database
-- CREATE DATABASE environmentdb;

-- -- Create the user
-- CREATE USER app_user WITH PASSWORD 'testing';

-- -- Grant privileges to the user on the database
-- GRANT ALL PRIVILEGES ON DATABASE environmentdb TO app_user;

-- -- Connect to the database
-- \connect environmentdb;

-- Create the tables
CREATE TABLE sensor (
    sensor_id SERIAL PRIMARY KEY,  -- Primary key for the sensor table
    sensor_type VARCHAR(255) UNIQUE NOT NULL  -- Column to be referenced by foreign key
);


CREATE TABLE hardware (
    hardware_id SERIAL PRIMARY KEY,  -- Primary key for the hardware table
    hardware_name VARCHAR(255) UNIQUE NOT NULL  -- Column to be referenced by foreign key
);


CREATE TABLE reading (
    reading_id SERIAL PRIMARY KEY,  -- Primary key for the reading table
    time TIMESTAMP NOT NULL,  -- Assuming time is a timestamp
    sensor_type VARCHAR(255) NOT NULL,
    hardware VARCHAR(255) NOT NULL,
    reading VARCHAR(255) NOT NULL,

    CONSTRAINT fk_sensor_type FOREIGN KEY (sensor_type) REFERENCES sensor (sensor_type),
    CONSTRAINT fk_hardware FOREIGN KEY (hardware) REFERENCES hardware (hardware_name)
);

--  `sudo apt install postgresql`

--  `createuser app_user -P --interactive`

--  `psql`

--  `create database environmentdb`

--  `\connect environmentdb`

--  `CREATE TABLE sensor (sensor_id SERIAL PRIMARY KEY, sensor_type VARCHAR(255) UNIQUE NOT NULL);`

--  `CREATE TABLE hardware (hardware_id SERIAL PRIMARY KEY, hardware_name VARCHAR(255) UNIQUE NOT NULL);`

--  `CREATE TABLE reading (reading_id SERIAL PRIMARY KEY, time TIMESTAMP NOT NULL, sensor_type VARCHAR(255) NOT NULL, hardware VARCHAR(255) NOT NULL, reading VARCHAR(255) NOT NULL, CONSTRAINT fk_sensor_type FOREIGN KEY (sensor_type) REFERENCES sensor (sensor_type), CONSTRAINT fk_hardware FOREIGN KEY (hardware) REFERENCES hardware (hardware_name));`


