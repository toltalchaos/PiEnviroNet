# PiEnviroNet
this app is for an ESP32 module to send data (main.cpp) for a light sensor to a database for later review!

## run flask app
`$ flask --app flask-backend/app run`

for running openly on local IP
`$ flask --app flask-backend/app run --host=0.0.0.0`

need to setup the following locally!

postgres db - environmentdb
user - app_user
pass - testing

