import psycopg2
import datetime



def insert_record(record):
    connection = psycopg2.connect(
    host='localhost',
    database='environmentdb',
    user= 'app_user',
    password= 'testing'
)
    curs = connection.cursor()

    curs.execute(f"INSERT INTO reading VALUES('{str(datetime.datetime.now())}', '{record}')")

    connection.commit()

    curs.close()
    connection.close()

def retrieve_readings():
    connection = psycopg2.connect(
    host='localhost',
    database='environmentdb',
    user= 'app_user',
    password= 'testing'
)
    curs = connection.cursor()
    curs.execute(f"SELECT * FROM reading")

    readings = curs.fetchall()

    curs.close()
    connection.close()

    return readings