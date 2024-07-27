# PiEnviroNet
this app is for an ESP32 module to send data (main.cpp) for a light sensor to a database for later review!

## Components:
- Flask API
    - this server is the running entryway for the ESP32 module to send data to and have it stored in the postgres db
- Postgres DB
    - the data storage vehicle for the long term data
    - need to setup the following locally! (see init.sql)
``` 
postgres db - environmentdb
user - app_user
pass - testing
```
    - `sudo apt install postgresql`
    - ` createuser app_user -P --interactive`
    - `psql`
    - `create database environmentdb`
    - `\connect environmentdb`
    - `
    - `CREATE TABLE sensor (sensor_id SERIAL PRIMARY KEY, sensor_type VARCHAR(255) UNIQUE NOT NULL);`
    - `CREATE TABLE hardware (hardware_id SERIAL PRIMARY KEY, hardware_name VARCHAR(255) UNIQUE NOT NULL);`
    - `CREATE TABLE reading (reading_id SERIAL PRIMARY KEY, time TIMESTAMP NOT NULL, sensor_type VARCHAR(255) NOT NULL, hardware VARCHAR(255) NOT NULL, reading VARCHAR(255) NOT NULL, CONSTRAINT fk_sensor_type FOREIGN KEY (sensor_type) REFERENCES sensor (sensor_type), CONSTRAINT fk_hardware FOREIGN KEY (hardware) REFERENCES hardware (hardware_name));`
- ESP32/arduino
    - the module or sensor control board (see esp-code/main.cpp) to run the logic
    - BME680 sensor board
        - this board is for humidity, temperature, and atmospheric pressure
    - photoresistor
        - light sensor
- Grafana
    - charting system available through local portal 
    - [docs HERE](https://raspberrytips.com/install-grafana-raspberry-pi/)

## run flask app
`$ flask --app flask-backend/app run`

for running openly on local IP
`$ flask --app flask-backend/app run --host=0.0.0.0`




## in the future it would be great to assign each ESP board a "location" and have a relational table in the DB for graphing stuff down the line
