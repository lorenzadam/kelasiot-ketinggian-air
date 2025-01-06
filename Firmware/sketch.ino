#include <LiquidCrystal_I2C.h>
#include <NusabotSimpleTimer.h>
#include <WiFi.h>
#include <MQTT.h>

LiquidCrystal_I2C lcd(0x27, 16,2);
NusabotSimpleTimer timer;
WiFiClient net;
MQTTClient client;

const int TRIG = 17;
const int ECHO = 16;
const int PIN_RED = 27;
const int PIN_GREEN = 26;
const int PIN_BLUE = 25;

const char ssid[] = "Wokwi-GUEST";
const char pass[] = "";

float jarak, ketinggian;
int kedalaman = 500;

void connect(){
  rgb(1,0,0);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }
  rgb(0,1,0);

  while(!client.connect("sungaiABC", "kelasiot", "passwordkelasiot")){
    delay(500);
  }
  client.subscribe("kelasiot/sungai/#");
  rgb(0,0,1);
}

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  lcd.init();
  lcd.backlight();
  WiFi.begin(ssid, pass);
  client.begin("kelasiot.cloud.shiftr.io", net);
  client.onMessage(subscribe);

  timer.setInterval(1000, sendData);

  connect();
}

void loop() {
  timer.run();
  client.loop();
  delay(10);
}

void readKetinggian(){
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  jarak = (0.0343 * pulseIn(ECHO, HIGH)) / 2;

  ketinggian = kedalaman - jarak;

  client.publish("kelasiot/sungai/ABC/ketinggian", String(ketinggian), true, 1);
}

void sendData(){
  readKetinggian();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ketinggan Air");

  lcd.setCursor(0,1);
  lcd.print(ketinggian);
  lcd.print(" cm");
}

void subscribe(String &topic, String &data){
  if(topic == "kelasiot/sungai/ABC/kedalaman"){
    kedalaman = data.toInt();
  }
}

void rgb(bool red, bool green, bool blue){
  digitalWrite(PIN_RED, red);
  digitalWrite(PIN_GREEN, green);
  digitalWrite(PIN_BLUE, blue);
}