#include "Arduino.h"
#include <WiFiClient.h>
#include "config.h"
#include "mqtt.h"
#include "pag1.h"
#include "pag2.h"
#include "pag_info.h"
#include <EspMQTTClient.h>
#include "SPIFFS.h"
#include "FS.h"

bool ap_mode = false;
const int ledpin = 3;
const int relaypin = 13;
const int resetpin = 21;
const int switchpin = 5;
bool ledState = HIGH;
int buttonState = 1;
int lastButtonState = 1;
unsigned long previousMillis = 0;
const long interval_ap = 1500;
const long interval_connection = 500;

void ap_blink();
void conf_reset();
void toggle();


void setup()
{
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
  pinMode(relaypin, OUTPUT);
  pinMode(resetpin, INPUT_PULLUP);
  pinMode(switchpin, INPUT_PULLUP);
  digitalWrite(ledpin, HIGH);
  digitalWrite(relaypin, LOW);
  check_data();
  
}

void loop()
{

  if (ap_mode == false)
  {
    client.loop();
  }
  if (ap_mode == true)
  {
    server.handleClient();
  }

  // ap_blink();
  conf_reset();
  // toggle();
}

void ap_blink()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval_ap)
  {
    previousMillis = currentMillis;
    if (ap_mode == true)
    {
      digitalWrite(ledpin, LOW);
      delay(10);
    }
  }
  if (ap_mode == true)
  {
    digitalWrite(ledpin, HIGH);
  }
}

void connection_blink()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval_connection)
  {
    previousMillis = currentMillis;
    digitalWrite(ledpin, LOW);
    delay(10);
    digitalWrite(ledpin, HIGH);
  }
}

void conf_reset()
{
  if (digitalRead(resetpin) == LOW)
  {
    for (int i = 0; i <= 5; i++)
    {
      digitalWrite(ledpin, LOW);
      delay(50);
      digitalWrite(ledpin, HIGH);
      delay(50);
    }
    SPIFFS.format();
    ESP.restart();
  }
}

void toggle()
{
  buttonState = digitalRead(switchpin);
  if (buttonState != lastButtonState)
  {
    if (buttonState == LOW)
    {
      digitalWrite(relaypin, !ledState);
      ledState = !ledState;
    }
    delay(100);
  }
  lastButtonState = buttonState;
}


