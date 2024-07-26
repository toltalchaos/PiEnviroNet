from flask import Flask, request, jsonify
from .db_functions import *
app= Flask(__name__)


@app.route("/")
def hello_world():
    return '<h1> Hello World! </h1>'

@app.route("/light-reading", methods=["POST"])
def recieve_reading():
# print out payload
    data = request.json
    print(data)
    insert_record(data.get('reading'), data.get('readingType'))
    if not data:
        return "invalid", 400
    return "recieved", 200

@app.route("/review")
def review():
    readings = retrieve_readings()
    return jsonify(readings)

@app.route("/review/<reading_type>")
def review_type(reading_type):
    readings = retrieve_readings(reading_type)
    return jsonify(readings)
    

if __name__ == '__main__':
    app.run(debug=True)