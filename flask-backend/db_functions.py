import psycopg2
import datetime

def get_connection():
    return psycopg2.connect(
    host='localhost',
    database='environmentdb',
    user= 'app_user',
    password= 'testing'
)

def insert_record(reading, reading_type):
    connection = get_connection()
    curs = connection.cursor()

    curs.execute(f"INSERT INTO reading (time, reading_type, reading) VALUES('{str(datetime.datetime.now())}','{reading_type}', '{reading}')")

    connection.commit()

    curs.close()
    connection.close()

def retrieve_readings(reading_type=None):
    connection = get_connection()

    if reading_type:
        curs = connection.cursor()
        curs.execute(f"SELECT * FROM reading WHERE reading_type = '{reading_type}'")
    else:
        curs = connection.cursor()
        curs.execute(f"SELECT * FROM reading")

    readings = curs.fetchall()

    curs.close()
    connection.close()

    return readings