// MQ-2 sensor working
// Temperature and Humidity sensor working
// LCD displaying values okay
// Buzzer triggered on event(s)
// Potentiometer works fine in tuning the brightness of the LCD

// DHT11 sensor

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(5,6); //RX, TX
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#include "DHT.h"
#include <MQ2.h>

#define DHTPIN 7 // what digital pin we're connected to
#define DHTTYPE DHT11  //DHT 11
#define GAS_SENSOR A0
#define FIRE_SENSE 4
int flame_detected;
const char* flame;

#define BUZZER 9

#define mq2_threshold 100
#define temp_threshold 40

int lpg, co, smoke;
MQ2 mq2(GAS_SENSOR);

DHT dht (DHTPIN, DHTTYPE); //Initialize DHT sensor.

// Set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {

s.begin(115200);
Serial.begin(9600);
pinMode(BUZZER, OUTPUT);
pinMode(FIRE_SENSE, INPUT);

dht.begin();
mq2.begin();


lcd.begin();
lcd.backlight();

lcd.setCursor(0,0);
lcd.print("******WELCOME!******");
lcd.setCursor(0,1);
lcd.print("IoT PROJECT ~CS890EP");
lcd.setCursor(0,2);
lcd.print("***Sibdow Iddrisu***");
lcd.setCursor(0,3);
lcd.print("***Aymen Ben-Said***");

//delay(5000);

}

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
 
void loop() {

float* values = mq2.read(true);
lpg = mq2.readLPG();
co = mq2.readCO();
smoke = mq2.readSmoke();
 
float t = dht.readTemperature(); // Read temperature in *C (default)
float h = dht.readHumidity(); // Read humidity %

float hic = dht.computeHeatIndex(t, h);

flame_detected = digitalRead(FIRE_SENSE);
if(flame_detected == 1){
  flame = "NO";
  }else{
    flame = "YES";
    }

Serial.print("FIRE: ");
Serial.print(flame_detected);
Serial.print(";\t");
Serial.println(flame);


if (isnan(t) || isnan(h) || isnan(hic)) {
    return;
  }
  
//-----------------------------------------------------------------------------------------------//
  root["temperature"] = t;
  root["humidity"] = h;  
  root["lpg"] = lpg;
  root["smoke"] = smoke;
  root["fire"] = flame_detected;

  if(s.available()>0)
    {
      root.printTo(s);
   }
//  delay(3000);
  
//-----------------------------------------------------------------------------------------------//
  
if (lpg > mq2_threshold || smoke > mq2_threshold || t > temp_threshold || flame_detected == 0) { // If the current readings exceed this limit, then turn on buzzer
  tone(BUZZER, 1000);
} else {
    noTone(BUZZER);
    }

Serial.print("Temerature: ");
Serial.print(t);
Serial.print("*C\t");
Serial.print("Humidity: ");
Serial.print(h);
Serial.print("%\t");
Serial.print("Heat index: ");
Serial.println(hic);




// LCD display

lcd.begin(); // display diamensions
lcd.backlight();
lcd.setCursor(0,0);
lcd.print("Temp=");
lcd.setCursor(5,0);
lcd.print(t);
lcd.setCursor(7,0);
lcd.print("*C ");
lcd.setCursor(11,0);
lcd.print("Hum=");
lcd.setCursor(15,0);
lcd.print(h);
lcd.setCursor(17,0);
lcd.print("%  ");

//===============================================//

lcd.setCursor(0,1);
lcd.print("LPG=");
lcd.setCursor(4,1);
lcd.print(lpg);
lcd.setCursor(13,1);
lcd.print("PPM");
lcd.setCursor(0,2);
lcd.print("SMK=");
lcd.setCursor(4,2);
lcd.print(smoke);
lcd.setCursor(13,2);
lcd.print("PPM");

if(lpg > mq2_threshold || smoke > mq2_threshold || flame_detected == 0) { // Current thresholds set at only 100 PPM for testing purposes
  if(flame_detected == 0){
    lcd.setCursor(0,3);
    lcd.print("Possible Fire >Flame");
    }else {
      lcd.setCursor(0,3);
      lcd.print("Possible Fire");
      }
  
  }
  else{
    lcd.setCursor(0,3);
    lcd.print("No Fire Detected");
    }

}
