#include <WiFi.h>  
#include "ThingSpeak.h"
#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <BH1750.h>

//import library of OLED screen
#include <U8x8lib.h> 
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15, 4, 16); // clock, data, reset

char ssid[] = "HONOR View20";                          //  your network SSID (name)
char pass[] = "Cocodile";                 // your network passw 
unsigned long myChannelNumber = 2102609;          // your channel 
const char * myWriteAPIKey="UZ1PC0QQEGOIZVG3";    // your API key
WiFiClient  client;
BH1750 lightMeter;
HTU21D myTempHumi;

bool MOTION_DETECTED = false;
#define PIR 17 // pin motion sensor


void pinChanged() {
  MOTION_DETECTED = true;
}

void setup() {
  myTempHumi.begin(); // initialize HTU21D
  lightMeter.begin(); // initialize BH1750
  attachInterrupt(PIR, pinChanged, RISING);

  Serial.begin(9600); 
  
  WiFi.disconnect(true);  // delete of EEPROM WiFi credentials 
  delay(1000);
  WiFi.begin(ssid, pass);
  delay(1000);      
  while (WiFi.status() != WL_CONNECTED) { // Setup Wifi
    delay(500); 
    Serial.print(".");
  } 
  IPAddress ip = WiFi.localIP(); 
  Serial.print("IP Address: "); 
  Serial.println(ip); 
  Serial.println("WiFi setup ok"); 
  delay(1000); 
 
  ThingSpeak.begin(client); // connexion (TCP) du client au serveur 
  delay(1000); 
  Serial.println("ThingSpeak begin");
}

void loop() {

// if motion detected, value =1 and reset to 0;
  if (MOTION_DETECTED){
    ThingSpeak.setField(5, 1);  // set up the field5
    Serial.println("Motion Detected");
    MOTION_DETECTED = false;
  }else{
    ThingSpeak.setField(5, 0);  // we still see if there is no movement
  }

// Setup data
  float humidity = myTempHumi.readHumidity();
  float temperature = myTempHumi.readTemperature();
  uint16_t luminosite = lightMeter.readLightLevel();
  
  Serial.print("Light: ");
  Serial.print(luminosite);
  Serial.print(" lx");
  Serial.print(" Temperature:");
  Serial.print(temperature, 1);
  Serial.print("C");
  Serial.print(" Humidity:");
  Serial.print(humidity, 1);
  Serial.print("%");
  Serial.println();

// Thingspeak field to send each data
  ThingSpeak.setField(6, luminosite);
  ThingSpeak.setField(7, temperature);
  ThingSpeak.setField(8, humidity);
 

  // In case of deconnection
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print("wait");
  }
  
  //Send Data to Thingspeak
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  delay(3500);
}
