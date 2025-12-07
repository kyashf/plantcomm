#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>    

const char* ssid = "Node"
const char* password = "12345678"

int recivePin = A0;
int transpin = D0; 
int led1 = 5;
int led2 = 4;
int ledWifi = 16;
int ledSystem = 2;
int butonPin = 14;

ESP8266WebServer server(80);

int signalBuffer[500];
int indexx = 0;
int threshold = 50;
int transmitCounter = 0;
int reciveCounter = 0;
bool sysLedState = false;
unsigned long timeNow = 0;
unsigned long lastTime = 0;

String webpage = "<html><body><h1>Plant Communication</h1><p>Working...</p></body></html>"

void setup(){

Serial.begin(9600);

Serial.println("Starting Something.......");
Serial.println("Waiting.... ");
delay(2000);

pinMode(transpin, OUTPUT);
pinMode(led1, OUTPUT);
pinMode(led2, OUTPUT);
pinMode(ledWifi, OUTPUT);
pinMode(butonPin, INPUT);

digitalWrite(transpin, 0);
digitalWrite(led1, 0);
digitalWrite(led2, 0);
digitalWrite(ledWifi, 0);

digitalWrite(ledSystem, HIGH);
delay(500);
digitalWrite(ledSystem, LOW);
delay(500);

Serial.println("Connecting to WiFi.........");
WiFi.begin(ssid, password);

WiFi.begin(ssid, password);

int count = 0;
while(WiFi.status() != WL_CONNECTED && count < 50)
{
  digitalWrite(ledWifi, !digitalRead(ledWifi));
  Serial.print(".");
  delay(300);
  count++;
}

if(WiFi.status() == WL_CONNECTED){
  Serial.println(" Connected!");
  digitalWrite(ledWifi, HIGH)
} 
else {
  Serial.println(" Not Connected :(");
  digitalWrite(ledWifi, HIGH);
}

server.on("/", [](){
  server.send(200, "text/html", webpage);
});

server.on("/data", [](){
  int v = analogRead(recivePin);

  String back = "{";
  back += "\"value\":" + String(v);
  back += "\"tx\":" + String(transmitCounter);
  back += "\"rx\":" + String(reciveCounter);
  back += "}";

  server.send(200, "application/json", back);
});

server.on("/send", [](){
  String type = server.arg("type");
  sendSignal(type);
  server.send(200,"text/plain","sent");
});

server.begin();

Serial.println("Server running on: ");
Serial.println(WiFi.localIP());

digitalWrite(ledSystem, HIGH);
}

void loop(){
server.handleClient();
checkButton();
readSignal();
blinks();
delay(2)
}

void sendSignal(String type){

Serial.println("Sending.... " + type);
transmitCounter++;

digitalWrite(transpin, HIGH);
delay(100);
digitalWrite(transpin, LOW);

digitalWrite(led2, HIGH);
delay(50);
digitalWrite(led2, LOW);
}

void checkButton(){
int reading = digitalRead(butonPin);

if(reading == 0){
 Serial.println("Button Pressed!!!");
 reciveCounter++;

 sendSignal("BUTTON")
 delay(10);
}
}

void readSignal(){
int val = analogRead(recivePin);

reciveCounter++;

if(val > threshold){
digitalWrite(led1, HIGH);
}
else {
digitalWrite(led1, LOW);
}
}

void blinks(){
unsigned long now = millis();

if(now - lastTime > 1500){
sysLedState = !sysLedState;
digitalWrite(ledSystem, sysLedState);
lastTime = now;
}
}
