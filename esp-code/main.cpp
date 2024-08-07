#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <array>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C default ESP32 I2C pins: GPIO 22 (SCL), GPIO 21 (SDA).

const String SENSOR_MODULE_NAME = "module_02_wiring_harness"; // NAME THIS, all readings FROM this device will be tied to this device.

// wifi settings
// const char WIFI_NAME[] = "Network Name";
// const char WIFI_PASSWORD[] = "vjpz8446";
const char WIFI_NAME[] = "TELUSWiFi3462";
const char WIFI_PASSWORD[] = "7M8B7zdVH7";

// server connection settings
String HOST_NAME = "nameofyour-pi.local:8080"; // unless this is pinned it will likely change
String READING_PATH_NAME = "/reading";
String HARDWARE_PATH_NAME = "/standup-hardware";

// new http client
HTTPClient http;

//whatever sensors are implemented should get added to this list for setting up in the DB
const String SENSOR_LIST[] = {"light", "humidity", "pressure", "temperature", "gas"};

// pin declarations
int lightSensor = 35; // i35 -> In 35
int ledOut = 2;       // IO2 -> pin2 (onboard LED)

void sendHardwarePayload();
void evaluateLight();
void evaluateAtmosphere();
void sendReading(String readingType, String readingValue);
void evaluateHttpResponse(int httpCode);

void setup() {
  delay(5000);
  Serial.begin(115200);      // repeat speed
  pinMode(ledOut, OUTPUT);   // set the IO pin as an output otherwise it will just receive power
  digitalWrite(ledOut, LOW); // turn the pin off

  // connect to wifi the server is hosted on
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
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
  
  Serial.begin(115200);

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

// THIS IS THE MAIN LOOP
void loop()
{
  digitalWrite(ledOut, HIGH);

  evaluateLight();
  evaluateAtmosphere();
  digitalWrite(ledOut, LOW); // turn the led off so we know readings have been sent

  delay(240000); // slow things down a bit
}

// light reading functions
void evaluateLight()
{
  int LightSensorAnalgValue = analogRead(lightSensor);
  Serial.printf("Light Value: %d \n", LightSensorAnalgValue);
  sendReading(SENSOR_LIST[0], String(LightSensorAnalgValue));
}

//atmosphere readings from BME680 chip
void evaluateAtmosphere(){
  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }

  //need to wait a few miliseconds so we can allow I2C comms + 400 JUUUUST in case
  delay(400 + (endTime - millis()));

  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

  Serial.print(F("Temperature = "));
  Serial.print(bme.temperature);
  Serial.println(F(" *C"));
  sendReading(SENSOR_LIST[3], String(bme.temperature));

  Serial.print(F("Pressure = "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));
  sendReading(SENSOR_LIST[2], String(bme.pressure));

  Serial.print(F("Humidity = "));
  Serial.print(bme.humidity);
  Serial.println(F(" %"));
  sendReading(SENSOR_LIST[1], String(bme.humidity));


  Serial.print(F("Gas = "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));
  sendReading(SENSOR_LIST[4], String(bme.gas_resistance));


  Serial.print(F("Approx. Altitude = "));
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(F(" m"));

  Serial.println();
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
      Serial.printf("[HTTP] ... failed, error: %s\n", http.errorToString(httpCode).c_str());
  } 
  http.end();
}

