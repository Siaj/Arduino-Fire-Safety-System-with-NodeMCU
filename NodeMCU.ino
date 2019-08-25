//#include <ESP8266WiFi.h>
// Thnger.io Specifications
#include <ThingerESP8266.h>

#include <SoftwareSerial.h>
SoftwareSerial s(D6,D5); //RX, TX
#include <ArduinoJson.h>



#define USERNAME "your_username"
#define DEVICE_ID "thinger_device_id"
#define DEVICE_CREDENTIAL "thinger_device_credentials"

#define SSID "your_wifi_ssid"
#define SSID_PASSWORD "your_wifi_password"


float t = 0;
float h = 0;
float lpg = 0;
float smk = 0;
int fire;
const char* flame_detected;

// Setup device details
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Setup WiFi
  thing.add_wifi(SSID, SSID_PASSWORD);

  
  Serial.begin(115200);
  s.begin(115200);
  while (!Serial) continue;

  // digital pin control example (i.e. turning on/off a light, a relay, configuring a parameter, etc)
  thing["led"] << digitalPin(LED_BUILTIN);

  // resource output example (i.e. reading a sensor value)
  thing["millis"] >> outputValue(millis());

  thing["jsonData"] >> [](pson& out) {
     out["temperature"] = t;
     out["humidity"] = h;
     out["lpg"] = lpg;
     out["smoke"] = smk;
     out["fire"] = flame_detected;
   };

}

unsigned long lastCheck = 0;
void loop() {
  // put your main code here, to run repeatedly:
  thing.handle();

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);

  if (root == JsonObject::invalid())
  {
    return;
  }

  
  
  t = root["temperature"];
  h = root["humidity"];
  lpg = root["lpg"];
  smk = root["smoke"];
  
  fire = root["fire"];
  if(fire == 1) {
    flame_detected = "NO";
    }else {
      flame_detected = "YES";
      }
  
  unsigned long currentTS = millis();
  Serial.println(lastCheck);
  Serial.println(currentTS);
  Serial.println(currentTS-lastCheck);
  if(currentTS-lastCheck>=1*60*1000) {
    lastCheck=currentTS;
    if (fire == 0) {
      // Call thinger.io email endpoint to send an email to address
      thing.call_endpoint("SIAJ");
      thing.call_endpoint("AYMEN");
      Serial.println(fire);
      Serial.println("Endpoint called");
      }
    }
}
