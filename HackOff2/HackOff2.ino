#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

DynamicJsonDocument doc(127);
DynamicJsonDocument ret(192);

const char* ssid = "Orchid";
const char* password = "2Two%Tang0";
float distance = 0.00;
float back_initial = 100; //Remember to change initial distance according to frame on hand
float angle1 = 0.00;
float angle2 = 0.00;
int torque = 0;

String ser = "http://ec2-13-233-63-133.ap-south-1.compute.amazonaws.com:8080/";
String status = ser + "status";
String arm = ser + "realtime_angle";
String arm_exc = ser + "angle";
String bac = ser + "realtime_curve";
String bac_exc = ser + "curve";
String leg = ser + "realtime_leg";
String leg_exc = ser + "leg";


Servo servo1, servo2;
Servo servo3; //Comment this line if using stepper for leg

const int step1 = D3;
const int dir1 = D2;
//const int step2 = D6; //Uncomment if using stepper for leg
//const int dir2 = D5; //Uncomment if using stepper for leg
const int steps_per_revolution = 400; //Set MS1 to HIGH, MS2 and MS3 to LOW

const int trig = D1;
const int echo = D0;

String getJSONData(String serverName)
{
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  HTTPClient http;
  http.begin(serverName);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) 
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
    if(httpResponseCode == 200)
    {
      return payload;
    }
  }
  else 
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return "Fail";
}

void extractJson(String payload)
{
  deserializeJson(ret, payload);
  JsonObject data_0 = doc["data"][0];
  int angle = data_0["angle"];
  int no = data_0["no"];
  int torque = data_0["torque"];
  Serial.println(angle);
  Serial.println(torque);
  Serial.println(no);
}

void postPainData(String serverName)
{
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  String data;
  doc["torque"] = torque;
  doc["current_hand_angle"] = angle1;
  doc["current_leg_angle"] = angle2;
  doc["back_arch_distance"] = distance;
  serializeJson(doc, data);
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(data);
  const String& payload = http.getString();
  http.end();
}

float getDistance()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  float duration = pulseIn(echo, HIGH);
  float dist = duration * 0.034 / 2;
  return dist;
}

int resToAngle()
{
  int resistance = analogRead(A0);
  if(resistance < 10)
    resistance = 10;
  int ang = map(resistance, 10, 670, 0, 180);
  return ang;
}

void initialise()
{
  servo1.write(0);
  servo2.write(0);
  servo3.write(0); //Comment this line if using stepper for leg

  digitalWrite(dir1, HIGH); //Change to LOW if direction is wrong
  while(distance < back_initial)
  {
    distance = getDistance();
    digitalWrite(step1, HIGH);
    delayMicroseconds(5);
    digitalWrite(step1, LOW);
  }

  /*digitalWrite(dir2, HIGH); //Change to LOW if direction is wrong
  while(resToAngle > 0)
  {
    distance = getDistance();
    digitalWrite(step2, HIGH);
    delayMicroseconds(5);
    digitalWrite(step2, LOW);
  }*/    //Uncomment block if using Stepper for leg
}

void setup() 
{
  Serial.begin(115200);

  servo1.attach(D8);
  servo2.attach(D7);
  servo3.attach(D6); //Comment this line if using stepper for leg

  pinMode(step1, OUTPUT);
  pinMode(dir1, OUTPUT);
  //pinMode(step2, OUTPUT); //Uncomment if using stepper for leg
  //pinMode(dir2, OUTPUT); //Uncomment if using stepper for leg

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  initialise();
}

void loop() 
{

}
