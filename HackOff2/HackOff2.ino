#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

DynamicJsonDocument ret(192);
DynamicJsonDocument ang(16);
DynamicJsonDocument dist(16);

float distance = 0.00;
float back_initial = 100; //Remember to change initial distance according to frame on hand
unsigned int angle1 = 0;
unsigned int angle2 = 0;
unsigned int angle3 = 0;
unsigned int torque = 0;
String s;
unsigned int mode = 0;
unsigned int inst = 0;
unsigned long current_time;

const char* ssid = "Orchid";
const char* password = "2Two%Tang0";
const String ser = "http://ec2-13-233-63-133.ap-south-1.compute.amazonaws.com:8080/";
const String status = ser + "status";
const String arm = ser + "realtime_angle";
const String arm_exc = ser + "angle";
const String bac = ser + "realtime_curve";
const String bac_exc = ser + "curve";
const String leg = ser + "realtime_leg";
const String leg_exc = ser + "leg";
const String hnd = ser + "realtime_hand";
const String hnd_exc = ser + "hand";

Servo servo1, servo2;
//Servo servo3; //Comment this line if using stepper for leg

const int step1 = D3;
const int dir1 = D2;
const int step2 = D6; //Uncomment if using stepper for leg
const int dir2 = D5; //Uncomment if using stepper for leg
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

void postData(String serverName, String data)
{
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
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
} //Uncomment block if using stepper for leg

void initialise()
{
  angle1 = 0;
  angle2 = 0;
  angle3 = 0;
  mode = 0;
  inst = 0;

  servo1.write(0);
  servo2.write(0);
  //servo3.write(0); //Comment this line if using stepper for leg

  digitalWrite(dir1, HIGH); //Change to LOW if direction is wrong
  while(distance < back_initial)
  {
    distance = getDistance();
    digitalWrite(step1, HIGH);
    delayMicroseconds(5);
    digitalWrite(step1, LOW);
  }

  digitalWrite(dir2, HIGH); //Change to LOW if direction is wrong
  while(resToAngle > 0)
  {
    distance = getDistance();
    digitalWrite(step2, HIGH);
    delayMicroseconds(5);
    digitalWrite(step2, LOW);
  }   //Uncomment block if using Stepper for leg
}

void setup() 
{
  Serial.begin(115200);

  servo1.attach(D8); //servo 1 connected to arm
  servo2.attach(D7); //servo 2 connected to hand
  //servo3.attach(D6); //Comment this line if using stepper for leg

  pinMode(step1, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(step2, OUTPUT); //Uncomment if using stepper for leg
  pinMode(dir2, OUTPUT); //Uncomment if using stepper for leg

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(D4, INPUT);

  initialise();
}

void loop() 
{
  if(mode == 0)
  {
    s = getJSONData(status);
    Serial.println(s);
    deserializeJson(ret, s);
    String NewData = ret["NewData"];
    if(NewData == "Yes")
    {
      mode = ret["Part"];
      inst = ret["Inst"];
    }
  }
  else if(mode == 1)
  {
    if(inst == 1)
    {
      Serial.println("Arm angle measurement mode");
      s = getJSONData(arm);
      if(s == "Yes")
      {
        current_time = millis();
        while(1)
        {
          if(millis() - current_time > 500)
          {
            angle1++;
            servo1.write(angle1);
            ang["angle"] = angle1;
            serializeJson(ang, s);
            postData(arm, s);
            current_time = millis();
          }
          if(digitalRead(D4) == HIGH)
          {
            initialise();
            break;
          }
        }
      }
    }
    else if(inst == 2)
    {
      Serial.println("Arm exercise Mode");
    }
  }
  else if(mode == 2)
  {
    if(inst == 1)
    {
      Serial.println("Back curve measurement mode");
      s = getJSONData(bac);
      if(s == "Yes")
      {
        current_time = millis();
        digitalWrite(dir1, LOW); //Change if stepper is in wrong direction
        while(1)
        {
          if(millis() - current_time > 500)
          {
            distance = getDistance();
            dist["distance"] = distance;
            serializeJson(dist, s);
            postData(bac, s);
            current_time = millis();
          }
          digitalWrite(step1, HIGH);
          delayMicroseconds(5);
          digitalWrite(step1, LOW);
          delayMicroseconds(5);
          if(digitalRead(D4) == HIGH)
          {
            dist["distance"] = distance;
            serializeJson(dist, s);
            postData(bac, s);
            initialise();
            break;
          }
        }
      }
    }
    else if(inst == 2)
    {
      Serial.println("Back exercise Mode");
    }
  }
  else if(mode == 3)
  {
    /*if(inst == 1)
    {
      Serial.println("Leg angle measurement mode");
      s = getJSONData(leg);
      if(s == "Yes")
      {
        current_time = millis();
        while(1)
        {
          if(millis() - current_time > 500)
          {
            angle2++;
            servo3.write(angle2);
            ang["angle"] = angle2;
            serializeJson(ang, s);
            postData(leg, s);
            current_time = millis();
          }
          if(digitalRead(D4) == HIGH)
          {
            mode = 0;
            inst = 0;
            break;
          }
        }
      }*/ //Uncomment block if using servo for leg
    if(inst == 1)
    {
      Serial.println("Leg angle measurement mode");
      s = getJSONData(bac);
      if(s == "Yes")
      {
        current_time = millis();
        digitalWrite(dir1, LOW); //Change if stepper is in wrong direction
        while(1)
        {
          if(millis() - current_time > 500)
          {
            digitalWrite(step1, HIGH);
            delayMicroseconds(5);
            digitalWrite(step1, LOW);
            delayMicroseconds(5);
            angle2 = resToAngle();
            ang["angle"] = angle1;
            serializeJson(ang, s);
            postData(leg, s);
            current_time = millis();
          }
          if(digitalRead(D4) == HIGH)
          {
            initialise();
            break;
          }
        }
      }
    }
    else if(inst == 2)
    {
      Serial.println("Leg exercise Mode");
    }
  }
  else if(mode == 4)
  {
    if(inst == 1)
    {
      Serial.println("Hand angle measurement mode");
      s = getJSONData(hnd);
      if(s == "Yes")
      {
        current_time = millis();
        while(1)
        {
          if(millis() - current_time > 500)
          {
            angle3++;
            servo1.write(angle3);
            ang["angle"] = angle3;
            serializeJson(ang, s);
            postData(hnd, s);
            current_time = millis();
          }
          if(digitalRead(D4) == HIGH)
          {
            initialise();
            break;
          }
        }
      }
    }
    else if(inst == 2)
    {
      Serial.println("Hand exercise Mode");
    }
  }
}
