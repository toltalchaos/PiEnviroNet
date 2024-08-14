
# PiEnviroNet

**PiEnviroNet** is an application designed to collect data from an ESP32 module equipped with various sensors, storing this data in a PostgreSQL database. The data can then be reviewed using a locally hosted Grafana server. This application is designed for easy setup and operation, including Docker support for lightweight deployment across a Local Area Network (LAN).

## Key Components

### 1. Flask API
The Flask API acts as a server, receiving data from the ESP32 module and storing it in a PostgreSQL database.

### 2. PostgreSQL Database
The PostgreSQL database is used to store sensor data for long-term analysis.

- **Setup Instructions:**
    - Follow the instructions in `init.sql` to set up the database locally.
    - If you prefer to run your database on an external hard drive, refer to [this resource](https://dba.stackexchange.com/questions/283845/postgresql-on-external-hd). Ensure that the database connection is properly configured in `db_functions.py` (`get_connection()`).
    - If using Docker, place this repository in the desired location for database storage. The volume library will be located in `/flask-backend/data/`.

- **Database Credentials:**
    - **Database Name:** mydatabase
    - **Username:** postgres
    - **Password:** password

### 3. ESP32/Arduino
The ESP32 module is used to run the sensor logic.

- **Configuration:**
    - Configuration options are located at the top of `esp-code/main.cpp`. The provided settings are tailored to a specific use case but can be easily modified.
    - The `ledOut` pin is configured to act as a heartbeat indicator. A blinking LED signifies normal operation, while a constant on or off state may indicate an issue.

- **Supported Sensors:**
    - **Light Sensor (Photoresistor):** [Documentation](https://docs.sunfounder.com/projects/esp32-starter-kit/en/latest/arduino/basic_projects/ar_photoresistor.html)
    - **Temperature & Humidity Sensor (DH11):** [Documentation](https://docs.sunfounder.com/projects/esp32-starter-kit/en/latest/arduino/basic_projects/ar_dht11.html)
    - **Temperature, Pressure, Altitude & Humidity Sensor (BME680):** [Documentation](https://randomnerdtutorials.com/esp32-bme680-sensor-arduino/)

### 4. Grafana
Grafana is used for visualizing the data collected by the sensors.

- **Start Grafana Server:**
    - `sudo /bin/systemctl start grafana-server`
    - Check status with `sudo /bin/systemctl status grafana-server`
    - Access Grafana at `http://<pi's local IP>:3000`
  
- **Default Login:**
    - **Username:** admin
    - **Password:** admin
  
- **Setup:**
    - Set up a connection with `localhost:6000` using the application’s username and password.
    - Additional setup instructions can be found in this [guide](https://raspberrytips.com/install-grafana-raspberry-pi/).

    - dont worry too much just setup a connection with `localhost:6000` and the application username and password
    ![configuration image](image.png)

## Running the Flask App

To start the Flask app locally, use the following command:

```sh
$ flask --app flask-backend/app run
```

To run the Flask app openly on your local IP address, use:

```sh
$ flask --app flask-backend/app run --host=0.0.0.0
```

## Running Flask in Docker

Docker allows you to manage and run the application in a lightweight, containerized environment.

- **Setup Docker:** Follow [this guide](https://raspberrytips.com/docker-on-raspberry-pi/) to set up Docker.
- **Running the Application:**
    1. Check for running Docker images: `docker ps`
    2. Navigate to the Flask directory: `cd flask-backend`
    3. Build and start the Docker containers: `docker compose up -d`
    4. Verify the running image: `docker ps` (You should be able to access the Flask app at `rasberrypi.local:8080`)
    5. Stop the containers: `docker compose down`

- **Managing Docker:**
    - View logs: `docker logs [image ID]`
    - Stop a container: `docker stop [image ID]`

- **Connect to the PostgreSQL Database in Docker:**
    ```sh
    psql -h localhost -p 6000 -U postgres
    ```
    - **Password:** postgres