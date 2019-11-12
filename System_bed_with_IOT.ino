 //#include <time.h> 
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TridentTD_LineNotify.h>

char auth[] = "NbmAStvN5m4OXa3MqtALy5ElG4SNVafA";
char ssid[] = "Jutamas Soiraya";
char pass[] = "0814242933";

#define BLYNK_PRINT Serial
#define DHTPIN 12
#define DHTTYPE DHT22
#define sensorPin A0

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
WidgetLCD lcd(V12);

int air = 5; //D1 - แอร์
int warmlight = 4; //D2 - วอม
int light = 14; //D5 - ไฟห้อง

bool sleepStatus = false;
bool startAutoSystem = false ;
bool warmlightStatus = true;
bool systemMode = false;
bool statusSwitchAir = false;
bool statusSwitchLight = false;
bool statusSwitchWarmLight = false;

float widgetSendTemp ; 

//Time Monitor
BLYNK_WRITE(V0){
  Serial.println("-Time Monitor-");
  if ( param.asInt() == 0 ){
    LINE.notify("ระบบเริ่มเปิดทำงาน");
    startAutoSystem = true;
    systemMode = true;
    digitalWrite(light,LOW); //open
  } else {
    LINE.notify("ระบบหยุดการทำงาน");
    startAutoSystem = false;
    digitalWrite(warmlight,LOW);
  }
}

//System Mode
BLYNK_WRITE(V1){
  Serial.println("-System Mode-");
  if ( param.asInt() == 1 ){
    systemMode = true;
  } else {
    systemMode = false;
  }
}

//Switch Air
BLYNK_WRITE(V2){
  Serial.println("-Switch Air-");
  if ( param.asInt() == 1 ){
    statusSwitchAir = true ;
  } else {
    statusSwitchAir = false ;
  }
}

//Switch Light
BLYNK_WRITE(V3){
  Serial.println("-Switch Light-");
  if ( param.asInt() == 1 ){
    statusSwitchLight = true ;
  } else {
    statusSwitchLight = false ;
  }
}

//Switch WarmLight
BLYNK_WRITE(V4){
  Serial.println("-Switch WarmLight-");
  if ( param.asInt() == 0 ){
    statusSwitchWarmLight = true ;
  } else {
    statusSwitchWarmLight = false ;
  }
}

void setup() {
  LINE.setToken("fT6aoFVE3HXj4ZD4YaymFwY4ZDb4ncdbGE9Y1N1eNj4");
  Serial.begin(9600);
  pinMode(air,OUTPUT);
  pinMode(warmlight,OUTPUT);
  pinMode(light,OUTPUT);
  digitalWrite(air,HIGH);
  digitalWrite(warmlight,LOW);
  digitalWrite(light,HIGH);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendTemperature);
  dht.begin();
}

//-----------------------------------------------------------------------
void loop() {
  Blynk.run();
  timer.run();

  float sensorTemperature = dht.readTemperature();
  widgetSendTemp = sensorTemperature;
  Serial.println("SensorTemperature : " + String(sensorTemperature));
   
  int sensorForce = analogRead(sensorPin);
  Serial.println("SensorForce : " + String(sensorForce));

  if (sensorTemperature > 28){
    openAir(true);
  }

  if (startAutoSystem == true && systemMode == true){ //  Auto Mode -
    Serial.println("Mode : " + String(systemMode) + " Auto");

    if ( sensorForce > 100){
      delay(5000);
      openLight(false);
      sleepStatus = true;
    }

    if (sleepStatus == true && sensorForce < 100){
      openWarmLight(true);
    } else {
      openWarmLight(false);
    }

  } else if ( systemMode == false ){ //  Manul Mode -
    Serial.println("Mode : " + String(systemMode) + " Manul");
    openAir(statusSwitchAir);
    openLight(statusSwitchLight);
    openWarmLight(statusSwitchWarmLight);
  } else {
    Serial.println("Mode : Auto");
  }

  Serial.println("\n -------------------------------------- \n");
  delay(500);
}
//-----------------------------------------------------------------------

void openAir(bool statusAir){
  if ( statusAir == true ){
    digitalWrite(air, LOW);
  } else {
    digitalWrite(air, HIGH);
  }
}

void openLight(bool statusLight){
  if ( statusLight == true ){
    digitalWrite(light, LOW);
  } else {
    digitalWrite(light, HIGH);
  }
}

void openWarmLight(bool statusWarm){
  if ( statusWarm == true ){
    digitalWrite(warmlight, HIGH);
  } else {
    digitalWrite(warmlight, LOW);
  }
}

void sendTemperature(){
  Blynk.virtualWrite(V12, widgetSendTemp);
}
