-- Create the database
CREATE DATABASE environmentdb;

-- Create the user
CREATE USER app_user WITH PASSWORD 'testing';

-- Grant privileges to the user on the database
GRANT ALL PRIVILEGES ON DATABASE environmentdb TO app_user;

-- Connect to the database
\c environmentdb;

-- Create the table
CREATE TABLE reading ( time VARCHAR(255), reading_type VARCHAR(255), reading VARCHAR(255) );

-- Insert some data
-- INSERT INTO reading (time, reading_type, reading) VALUES ('2020-01-01 00:00:00', 'temperature', '20');