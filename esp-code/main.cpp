#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//wifi settings
const char WIFI_NAME[] = "Network Name";
const char WIFI_PASSWORD[] = "vjpz8446";

//server connection settings
String HOST_NAME = "http://192.168.102.206:5000";
String PATH_NAME = "/light-reading";


//new http client
HTTPClient http;

//pin declarations
int lightSensor = 35; //i35 -> In 35
int ledOut = 26; //IO26 -> out 26

const int MINIMUM_LIGHT_THRESHOLD = 800;

void evaluateLight();
void itsDarkInHere(int readingValue);
void lightHasHappened();
void sendReading(Srting readingType, int readingValue);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //repeat speed
  pinMode(ledOut, OUTPUT); //set the IO pin as an output otherwise it will just recieve power

//connect to wifi the server is hosted on
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  Serial.println("CONNECTING....");
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print('.');
  }
  Serial.println("CONNECTED!!! local IP: " + WiFi.localIP());

 
  // try a GET on the root
  http.begin(HOST_NAME);
  int httpCode = http.GET();
  Serial.println(HOST_NAME + " resulted in : " + httpCode);
  http.end();
}

void loop() {
  delay(1000);//slow things down a bit

  evaluateLight();
}


//light reading functions
void evaluateLight(){
  int iteration = 0;
  array<int, 10> readings;
  do{
  int LightSensorAnalgValue = analogRead(lightSensor);
  readings[iteration] = LightSensorAnalgValue;
  iteration++;
  delay(30);
  }while(iteration < 10);

  int averageLightValue = 0;
  for(int i = 0; i < 10; i++){
    averageLightValue += readings[i];
  }
  averageLightValue = averageLightValue / 10;
  Serial.println("Light Value: " + averageLightValue);
  if( averageLightValue >= MINIMUM_LIGHT_THRESHOLD){
    itsDarkInHere(averageLightValue);
  }
  else{
    lightHasHappened();
  }
}

void itsDarkInHere(int readingValue){
    Serial.println("its dark in here");
    digitalWrite(ledOut, HIGH); //change the voltage level
    sendReading("light", readingValue);
}

void lightHasHappened(){
      digitalWrite(ledOut, LOW); //turn the pin off
}


//send reading to server
void sendReading(String readingType, int readingValue){
  StaticJsonDocument<500> doc;
  doc["reading"] = readingValue;
  doc["readingType"] = readingType;
  char jsonString[500];
  serializeJson(doc, jsonString);

  http.begin(HOST_NAME + PATH_NAME);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonString);
  http.end();
}