from flask import Flask, request, jsonify
from flask_cors import CORS

from .db_functions import *
app= Flask(__name__)
CORS(app)

@app.before_request
def log_request_info():
    print('Headers: %s', request.headers)
    print('Body: %s', request.get_data())



@app.route("/")
def hello_world():
    return '<h1> Hello World! </h1>'


@app.route('/test', methods=['GET'])
def test():
    return "Test successful"


@app.route("/reading", methods=["POST"])
def recieve_reading():
    '''
    {'reading': 1323, 'sensorType': 'light', 'moduleName': 'some_name'}
    '''
# print out payload
    data = request.json
    print(data)
    insert_record(data.get('reading'), data.get('sensorType'), data.get('moduleName'))
    if not data:
        return "invalid", 400
    return "recieved", 200

@app.route("/standup-hardware", methods=["POST"])
def standup_hardware():
    """
    {
    "hardwareName":"some_name",
    "sensorType":["light", "humidity", "pressure", "temperature"]
    }
    """
    try:
        data = request.json
        if not data:
            return jsonify({"error": "No input data provided"}), 400

        hardware_name = data.get('hardwareName')
        sensor_types = data.get('sensorType')

        if not hardware_name or not sensor_types:
            return jsonify({"error": "Missing hardwareName or sensorType"}), 400

        ensure_hardware(hardware_name)
        for sensor in sensor_types:
            ensure_sensor(sensor)

        return jsonify({"message": "Hardware setup successful"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route("/review")
def review():
    readings = retrieve_readings()
    return jsonify(readings)
    

if __name__ == '__main__':
    app.run(debug=True)