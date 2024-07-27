#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <array>

const String SENSOR_MODULE_NAME = "some_name"; // NAME THIS, all readings FROM this device will be tied to this device.

// wifi settings
const char WIFI_NAME[] = "Network Name";
const char WIFI_PASSWORD[] = "vjpz8446";

// const char WIFI_NAME[] = "TELUS0420";
// const char WIFI_PASSWORD[] = "t3hypycj5n";

// server connection settings
String HOST_NAME = "http://192.168.102.206:8080";
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

const int MINIMUM_LIGHT_THRESHOLD = 1000;

void sendHardwarePayload();
void evaluateLight();
void lightHasHappened();
void sendReading(String readingType, String readingValue);
void evaluateHttpResponse(int httpCode);

void setup() {
  Serial.begin(115200);      // repeat speed
  pinMode(ledOut, OUTPUT);   // set the IO pin as an output otherwise it will just receive power
  digitalWrite(ledOut, LOW); // turn the pin off

  // connect to wifi the server is hosted on
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  Serial.println("CONNECTING....");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println("CONNECTED!!!");

  //ensure we are assigned an IP
  Serial.println("ESP32 IP address: ");
Serial.println(WiFi.localIP());

  // try a GET on test
  http.begin(HOST_NAME + "/test");
  evaluateHttpResponse(http.GET());  

  sendHardwarePayload();
  digitalWrite(ledOut, HIGH);
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
    Serial.println("its dark in here");
   sendReading("light", String(LightSensorAnalgValue));
  }
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
  evaluateHttpResponse(http.POST(jsonString));
  
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

  // HTTPClient http;
  http.begin(HOST_NAME + HARDWARE_PATH_NAME);
  http.addHeader("Content-Type", "application/json");
  evaluateHttpResponse(http.POST(jsonString));
}

void evaluateHttpResponse(int httpCode){
  if (httpCode > 0) {
      Serial.printf("[HTTP Response]... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Received payload:");
        Serial.println(payload);
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  } 
  http.end();
}