#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <iostream>
using namespace std;

const char ssid[] = "Pixel_1850";
const char pass[] = "ainaaina";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

int tiltCaptor = D7;
int distanceCaptoreEcho = D2;
int distanceCaptorTrig = D3;
int photoCaptor = A0;
int whiteLed = D5;
int blueLed = D6;
int redLed = D4;
void connect() {
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  Serial.println("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("Trash", "cuteduck639", "trashcan")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/tilt");
  client.subscribe("/ultraSonic");
  client.subscribe("/photo");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void setup() {
  pinMode(tiltCaptor, INPUT);
  pinMode(distanceCaptorTrig, OUTPUT);
  pinMode(distanceCaptoreEcho, INPUT);
  pinMode(photoCaptor, INPUT);
  pinMode(whiteLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  client.begin("cuteduck639.cloud.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
  }

  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    bool tiltBool = digitalRead(tiltCaptor);
    string tiltValue = to_string(tiltBool);
    float distance = getDistanceCaptorValue(distanceCaptoreEcho, distanceCaptorTrig);
    string distanceValue = to_string(distance);
    float photoValue = map(analogRead(photoCaptor), 0, 1023, 0, 255);
    string photoString = to_string(photoValue);
    if(distance <= 10) {
      digitalWrite(whiteLed, HIGH);
    } else {
      digitalWrite(whiteLed, LOW);
    }
    if(photoValue >= 5) {
      digitalWrite(blueLed, HIGH);
    } else {
      digitalWrite(blueLed, LOW);
    }

    if(!tiltBool) {
      digitalWrite(redLed, HIGH);
    } else {
      digitalWrite(redLed, LOW);
    }

    client.publish("/tilt", tiltValue.c_str());
    client.publish("/ultraSonic", distanceValue.c_str());
    client.publish("/photo", photoString.c_str());
  }
}

float getDistanceCaptorValue(int echoPin, int trigPin) {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration_us = pulseIn(echoPin, HIGH);

  float distance_cm = 0.017 * duration_us;

  return distance_cm;
}


