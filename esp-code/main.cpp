#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <array>

const String SENSOR_MODULE_NAME = "some_name"; // NAME THIS, all readings FROM this device will be tied to this device.

// wifi settings
const char WIFI_NAME[] = "Network Name";
const char WIFI_PASSWORD[] = "vjpz8446";

// server connection settings
String HOST_NAME = "http://192.168.102.206:5000";
String READING_PATH_NAME = "/reading";
String HARDWARE_PATH_NAME = "/standup-hardware";

// new http client
HTTPClient http;

//whatever sensors are implemented should get added to this list for setting up in the DB
// const String SENSOR_LIST[] = {"light", "humidity", "pressure", "temperature"};
const String SENSOR_LIST[] = {"light"};//currently only have a light sensor

// pin declarations
int lightSensor = 35; // i35 -> In 35
int ledOut = 2;       // IO2 -> pin2 (onboard LED)

const int MINIMUM_LIGHT_THRESHOLD = 800;

void setupHardware();
void sendHardwarePayload();
void evaluateLight();
void itsDarkInHere(int readingValue);
void lightHasHappened();
void sendReading(String readingType, String readingValue);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);      // repeat speed
  pinMode(ledOut, OUTPUT);   // set the IO pin as an output otherwise it will just recieve power
  digitalWrite(ledOut, LOW); // turn the pin off

  // connect to wifi the server is hosted on
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  Serial.println("CONNECTING....");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print('.');
  }
  Serial.println("CONNECTED!!! local IP: " + WiFi.localIP());

  // try a GET on the root
  http.begin(HOST_NAME);
  int httpCode = http.GET();
  Serial.println(HOST_NAME + " resulted in : " + httpCode);
  http.end();
  setupHardware();
  digitalWrite(ledOut, HIGH);
}

// ensure the hardware is setup as all configuration should be done here
void setupHardware()
{
  // Your hardware setup code here

  // Send hardware setup payload
  sendHardwarePayload();
}

// THIS IS THE MAIN LOOP
void loop()
{
  digitalWrite(ledOut, HIGH);
  delay(5000); // slow things down a bit
  evaluateLight();
  digitalWrite(ledOut, LOW); // turn the pin off
  delay(5000);               // slow things down a bit
}

// light reading functions
void evaluateLight()
{
  int LightSensorAnalgValue = analogRead(lightSensor);
  Serial.printf("Light Value: %d \n", LightSensorAnalgValue);

  if (LightSensorAnalgValue >= MINIMUM_LIGHT_THRESHOLD)
  {
    itsDarkInHere(LightSensorAnalgValue);
  }
}

void itsDarkInHere(int readingValue)
{
  Serial.println("its dark in here");
  sendReading("light", String(readingValue));
}

// send reading to server, pass the sensor and the reading
void sendReading(String sensorType, String readingValue){
  StaticJsonDocument<500> doc;
  doc["reading"] = readingValue;
  doc["sensorType"] = sensorType;
  doc["moduleName"] = SENSOR_MODULE_NAME;
  char jsonString[500];
  serializeJson(doc, jsonString);

  http.begin(HOST_NAME + READING_PATH_NAME);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonString);
  if (httpCode > 0)
  {
    String payload = http.getString();
    Serial.println("HTTP Response code: " + String(httpCode));
    Serial.println("Response payload: " + payload);
  }
  else
  {
    Serial.println("Error on HTTP request");
    Serial.println("Response payload: " + http.getString());
  }
  http.end();
}

//on init we just check to make sure the DB is setup with references to our sensors.
void sendHardwarePayload()
{
  StaticJsonDocument<500> doc;
  doc["hardwareName"] = SENSOR_MODULE_NAME;
  JsonArray sensorTypes = doc.createNestedArray("sensorType");

  for (const String &sensor : SENSOR_LIST)
  {
    sensorTypes.add(sensor);
  }

  char jsonString[500];
  serializeJson(doc, jsonString);

  HTTPClient http;
  http.begin(HOST_NAME + HARDWARE_PATH_NAME);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonString);

  if (httpCode > 0)
  {
    String payload = http.getString();
    Serial.println("HTTP Response code: " + String(httpCode));
    Serial.println("Response payload: " + payload);
  }
  else
  {
    Serial.println("Error on HTTP request");
    Serial.println("Response payload: " + http.getString());
  }

  http.end();
}
