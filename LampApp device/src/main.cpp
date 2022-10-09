#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include "AiEsp32RotaryEncoder.h"
#include "AiEsp32RotaryEncoderNumberSelector.h"
#include "SPIFFS.h"
#include "FS.h"
#include "config.h"
#include "pag1.h"
#include "pag2.h"
#include "pag_info.h"
#include "access_point.h"

#define DEBUG

long firstPixelHue = 0;
int led_count = 0;
unsigned long t = 0;

String topic_string = "";
String topic_string_callback = "";
String mqtt_user = "";
String mqtt_password = "";
String nome_dispositivo = "";
String ssid = "";
String password = "";
String broker = "";
int port = 1883;
uint64_t chipid = ESP.getEfuseMac();
uint16_t chip = (uint16_t)(chipid >> 32);
String client_id = (String)chip;

bool rainbow_state = false;
int ring_state = 0;

const int resetpin = 19; 
bool connection_status = false;
bool ap_mode = false;
bool stato_led = false;
const long ap_timer = 500;
const long wifi_timer = 1500;
unsigned long previousMillis = 0;
unsigned long last_time = 0;

#define RING_PIN 15
#define NUM_LED 12
#define BRIGHTNESS 255 // 0 (min) to 255 (max)
Adafruit_NeoPixel ring(NUM_LED, RING_PIN, NEO_GRB + NEO_KHZ800);
int delayVal = 1;

#define ROTARY_ENCODER_A_PIN 21
#define ROTARY_ENCODER_B_PIN 22
#define ROTARY_ENCODER_BUTTON_PIN 23
#define ROTARY_ENCODER_STEPS 4
AiEsp32RotaryEncoder *rotaryEncoder = new AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);
AiEsp32RotaryEncoderNumberSelector numberSelector = AiEsp32RotaryEncoderNumberSelector();

WiFiClient espClient;
PubSubClient client(espClient);
long lastReconnectAttempt = 0;

      

void setup_wifi()
{
  WiFi.begin(ssid.c_str(), password.c_str());
  #ifdef DEBUG
  Serial.println("WiFi connecting...");
  #endif
  if (WiFi.status() != WL_CONNECTED)
  {
    #ifdef DEBUG
    Serial.println("WiFi connected");
    #endif
    delay(200);
  }
  // client.setServer(broker.c_str(), 1883);
}

void callback(char *topic, byte *message, unsigned int length)
{
  String messageString;
  for (int i = 0; i < length; i++)
  {
    messageString += (char)message[i];
  }

  if (String(topic) == topic_string)
  {
    if (messageString == "on")
    {
      #ifdef DEBUG
        Serial.println("Acceso");
      #endif
      ring_state = 1;
      rainbow_state = false;
      numberSelector.setRange(1, 255, 1, false, 0);
      numberSelector.setValue(255);
      client.publish(topic_string_callback.c_str(), "on", true);
      ring.setBrightness(255);
      for (int i = 0; i <= NUM_LED; i++)
      {
        ring.setPixelColor(i, ring.Color(255, 255, 255));
        ring.show();
        delay(delayVal);
      }
    }

    else if (messageString == "off")
    {
      #ifdef DEBUG
        Serial.println("Spento");
      #endif
      client.publish(topic_string_callback.c_str(), "off", true);
      rainbow_state = false;
      ring_state = 0;
      for (int i = 0; i <= NUM_LED; i++)
      {
        for (int i = 0; i <= NUM_LED; i++)
        {
          ring.setPixelColor(i, ring.Color(0, 0, 0));
          ring.show();
          delay(delayVal);
        }
      }
    }

    else if (messageString == "rainbow")
    {
      #ifdef DEBUG
        Serial.println("Rainbow");
      #endif
      client.publish(topic_string_callback.c_str(), "rainbow",true);
      rainbow_state = true;
      ring_state = 3;
      numberSelector.setRange(1, 255, 1, false, 0);
      numberSelector.setValue(255);
      rotaryEncoder->setAcceleration(200);
      ring.setBrightness(255);
      numberSelector.setValue(255);
    }

    else
    {
      #ifdef DEBUG
        Serial.println("Colore " + messageString);
      #endif
      client.publish(topic_string_callback.c_str(), "ok");
      rainbow_state = false;
      ring_state = 3;
      int r, g, b, bright;
      int n = sscanf(messageString.c_str(), "%d,%d,%d,%d", &r, &g, &b, &bright);
      int bright_correct = map(bright, 0, 255, 1, 255);
      numberSelector.setRange(1, 255, 1, false, 0);
      ring.setBrightness(bright_correct);
      numberSelector.setValue(bright_correct);
      for (int i = 0; i <= NUM_LED; i++)
      {
        ring.setPixelColor(i, ring.Color(r, g, b));
        ring.show();
        delay(delayVal);
      }
    }
  }
}

boolean reconnect()
{
  if (client.connect(client_id.c_str(),
                     "",
                     "",
                     topic_string_callback.c_str(),
                     0,
                     true,
                     "poff"))
  {
  }
  #ifdef DEBUG
  Serial.println("MQTT connecting...");
  #endif
  return client.connected();
}

void conf_reset()
{
  if (digitalRead(resetpin) == LOW)
  {
    for (int i = 0; i <= 5; i++)
    {
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(50);
    }
    SPIFFS.format();
    ESP.restart();
  }
}

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder->readEncoder_ISR();
}

void ap_blink()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= ap_timer)
  {
    previousMillis = currentMillis;
    digitalWrite(LED_BUILTIN, stato_led);
    stato_led = !stato_led;
  }
}

void status_blink()
{
  if (connection_status == false){
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= wifi_timer)
    {
      previousMillis = currentMillis;
      digitalWrite(LED_BUILTIN, stato_led);
      stato_led = !stato_led;
    }
  }
}

void read_encoder()
{
  if (rotaryEncoder->encoderChanged())
  {

    if (ring_state == 1)
    {
      int val = numberSelector.getValue();
      #ifdef DEBUG
        Serial.println(val);
      #endif
      ring.setBrightness(val);
      ring.show();
    }
    else if (ring_state == 2)
    {
      int val = numberSelector.getValue();
      #ifdef DEBUG
        Serial.println(val);
      #endif
      ring.setBrightness(255);
      for (int i = 0; i < ring.numPixels(); i++)
      {
        int pixelHue = val + (65536L / ring.numPixels());
        ring.setPixelColor(i, ring.gamma32(ring.ColorHSV(pixelHue)));
      }
      ring.show();
    }
    else if (ring_state == 3)
    {
      int val = numberSelector.getValue();
      #ifdef DEBUG
        Serial.println(val);
      #endif
      ring.setBrightness(val);
      ring.show();
    }
  }

  if (rotaryEncoder->isEncoderButtonClicked())
  {
    if (ring_state == 0)
    {
      #ifdef DEBUG
        Serial.println("Acceso");
      #endif
      numberSelector.setRange(1, 255, 1, false, 0);
      numberSelector.setValue(255);
      rotaryEncoder->setAcceleration(200); 
      ring_state = 1;
      rainbow_state = false;
      client.publish(topic_string_callback.c_str(), "on", true);
      ring.setBrightness(255);
      for (int i = 0; i <= NUM_LED; i++)
      {
        ring.setPixelColor(i, ring.Color(255, 255, 255));
        ring.show();
        delay(delayVal);
      }
    }
    else if (ring_state == 1)
    {
      #ifdef DEBUG
        Serial.println("Colore");
      #endif
      
      numberSelector.setRange(0, 65536, 500, true, 0);
      numberSelector.setValue(29000);
      rotaryEncoder->setAcceleration(100);
      ring_state = 2;
    }

    else if (ring_state == 2)
    {
      #ifdef DEBUG
        Serial.println("Spento");
      #endif
      ring_state = 0;
      rainbow_state = false;
      client.publish(topic_string_callback.c_str(), "off", true);
      for (int i = 0; i <= NUM_LED; i++)
      {
        ring.setPixelColor(i, ring.Color(0, 0, 0));
        ring.show();
        delay(delayVal);
      }
    }
    else if (ring_state == 3)
    {
      #ifdef DEBUG
        Serial.println("Spento");
      #endif
      numberSelector.setRange(1, 255, 1, false, 0);
      numberSelector.setValue(255);
      rotaryEncoder->setAcceleration(200);
      ring_state = 0;
      rainbow_state = false;
      client.publish(topic_string_callback.c_str(), "off", true);
      for (int i = 0; i <= NUM_LED; i++)
      {
        ring.setPixelColor(i, ring.Color(0, 0, 0));
        ring.show();
        delay(delayVal);
      }
    }
  }
}

void rainbow_effect()
{
  if (rainbow_state == true){   
    if (millis() > t + 60)
    {
      t = millis();
      for (int i = 0; i < ring.numPixels(); i++)
      {
        //int pixelHue = firstPixelHue + (i * 65536L / ring.numPixels());
        int pixelHue = firstPixelHue + (65536L / ring.numPixels());
        ring.setPixelColor(i, ring.gamma32(ring.ColorHSV(pixelHue)));
      }
      ring.show();
      if (firstPixelHue > 5 * 65536)
      {
        firstPixelHue = 0;
      }
      firstPixelHue += 256;
    }
  }
}

void setup()
{
  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  ring.begin();

  pinMode(resetpin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  rotaryEncoder->begin();
  rotaryEncoder->setup(readEncoderISR);
  numberSelector.attachEncoder(rotaryEncoder);
  numberSelector.setRange(1, 255, 1, false, 0);
  numberSelector.setValue(255);
  rotaryEncoder->setAcceleration(200); // 0 = disable accelleration, 250 max accelleration

  check_data();
  if (ap_mode == false)
  {
    setup_wifi();
  }
  client.setServer(broker.c_str(), 1883);
  client.setCallback(callback);
}

void loop()
{
  conf_reset();
  read_encoder();
  rainbow_effect();
  status_blink();

  if (ap_mode == true)
  {
    server.handleClient();
    ap_blink();
  }

  else if (ap_mode == false)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      unsigned long time = millis();
      if (time > last_time + 5000)
      {
        last_time = millis();
        #ifdef DEBUG
          Serial.println("WiFi connection lost. Retry in 5 sec.");
        #endif
        setup_wifi();
      }
      connection_status = false;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      if (!client.connected())
      {
        connection_status = false;
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000UL)
        {
          lastReconnectAttempt = now;
          #ifdef DEBUG
            Serial.println("MQTT connection lost");
          #endif
          if (reconnect())
          {
            #ifdef DEBUG
              Serial.println("MQTT connected");
            #endif
            connection_status = true;
            digitalWrite(LED_BUILTIN, HIGH);
            client.publish(topic_string_callback.c_str(), "pon", true);
            client.subscribe(topic_string.c_str());
            lastReconnectAttempt = 0;
          }
        }
      }
    }
    client.loop();
  }
}