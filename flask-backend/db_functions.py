import psycopg2
import datetime

def get_connection():
    return psycopg2.connect(
    host='localhost',
    database='environmentdb',
    user= 'app_user',
    password= 'testing'
)

def insert_record(reading, reading_type, hardware_name):
    connection = get_connection()  # Ensure this function returns a valid connection object
    curs = connection.cursor()

    # Insert the record into the reading table
    query = """
    INSERT INTO reading (time, sensor_type, hardware, reading)
    VALUES (%s, %s, %s, %s)
    """
    curs.execute(query, (datetime.datetime.now(), reading_type, hardware_name, reading))

    connection.commit()
    curs.close()
    connection.close()

def retrieve_readings(sensor_type=None):
    connection = get_connection()

    if sensor_type:
        curs = connection.cursor()
        curs.execute(f"SELECT * FROM reading WHERE sensor_type = '{sensor_type}'")
    else:
        curs = connection.cursor()
        curs.execute(f"SELECT * FROM reading")

    readings = curs.fetchall()

    curs.close()
    connection.close()

#    columns = [desc[0] for desc in curs.description]
#     readings = [dict(zip(columns, row)) for row in readings]
#     return json.dumps(readings)

    return readings


def ensure_hardware(hardware_name):
    connection = get_connection()
    curs = connection.cursor()

    # Check if the hardware_name exists
    curs.execute("SELECT COUNT(*) FROM hardware WHERE hardware_name = %s", (hardware_name,))
    if curs.fetchone()[0] == 0:
        # Insert the new hardware_name
        curs.execute("INSERT INTO hardware (hardware_name) VALUES (%s)", (hardware_name,))
        print(f"Inserted new hardware: {hardware_name}")

    connection.commit()
    curs.close()
    connection.close()

def ensure_sensor(sensor_type):
    connection = get_connection()
    curs = connection.cursor()

    # Check if the sensor_type exists
    curs.execute("SELECT COUNT(*) FROM sensor WHERE sensor_type = %s", (sensor_type,))
    if curs.fetchone()[0] == 0:
        # Insert the new sensor_type
        curs.execute("INSERT INTO sensor (sensor_type) VALUES (%s)", (sensor_type,))
        print(f"Inserted new sensor type: {sensor_type}")

    connection.commit()
    curs.close()
    connection.close()