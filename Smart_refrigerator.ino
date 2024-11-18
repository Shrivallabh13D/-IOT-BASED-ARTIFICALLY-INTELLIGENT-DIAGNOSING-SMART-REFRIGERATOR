#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <dht.h>
#include <stdlib.h>

#define dht_apin A0         // Analog Pin sensor is connected to DHT

dht DHT;

int gasSensor = 8;
int gasValue = 0;
String gasStatus;

int circuitSensor = 12;
int circuitValue = 0;
String circuitStatus;

const int BUZZER = 13;

String apiKey = "TV6R2I20X7N90GA3";

SoftwareSerial ser(10, 11); // RX, TX
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup() {
  pinMode(gasSensor, INPUT);
  pinMode(circuitSensor, INPUT);
  pinMode(BUZZER, OUTPUT);
  lcd.begin(16,2);
  StartUpMessage();
  
  // enable software serial
  ser.begin(9600);
  
  // reset ESP8266
  ser.println("AT+RST");
  delay(1000);
  SetParameters();
}


// the loop 
void loop() {
  
  DHT.read11(dht_apin);

  gasValue = 0;
  gasStatus = "L";
  if(!(digitalRead(gasSensor) == HIGH)){
    gasValue = 5;
    gasStatus = "H";
    startBuzzer();startBuzzer();
  }

  circuitValue = 0;
  circuitStatus = "OK  ";
  if(digitalRead(circuitSensor) == LOW){
    circuitValue = 5;
    circuitStatus = "FAIL";
    startBuzzer();startBuzzer();
  }
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);
   
  if(ser.find("Error")){    
    return;
  }
  
  // prepare GET string
  String getStr1 = "GET /update?api_key=";
  getStr1 += apiKey;
  getStr1 +="&field1=";
  getStr1 += String(DHT.temperature);
  getStr1 +="&field2=";
  getStr1 += String(DHT.humidity);
  getStr1 +="&field3=";
  getStr1 += String(gasValue);
  getStr1 +="&field4=";
  getStr1 += String(circuitValue);
  
  getStr1 += "\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr1.length());
  ser.println(cmd);

  if(ser.find(">")){
    ser.print(getStr1);
  }
  else{
    ser.println("AT+CIPCLOSE");    
  }
  DisplayParameters();
  delay(14000);   
}

void StartUpMessage(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("     SMART      ");
  lcd.setCursor(0,1);
  lcd.print("  REFRIGERATOR  ");
}
void SetParameters(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.setCursor(9,0);
  lcd.print("H:");
  lcd.setCursor(0,1);
  lcd.print("TOXICGAS:");
  lcd.setCursor(11,1);
  lcd.print("CS:");
}
void DisplayParameters(){
  lcd.setCursor(2,0);
  lcd.print(DHT.temperature);
  lcd.setCursor(11,0);
  lcd.print(DHT.humidity);
  lcd.setCursor(9,1);
  lcd.print(gasStatus);
  lcd.setCursor(14,1);
  lcd.print(circuitStatus);
}
void startBuzzer() {
  digitalWrite(BUZZER, HIGH); delay(500);
  digitalWrite(BUZZER, LOW); delay(300);
  digitalWrite(BUZZER, HIGH); delay(500);
  digitalWrite(BUZZER, LOW); delay(300);
}

