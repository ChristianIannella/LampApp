#ifndef mqtt_h
#define mqtt_h
#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"
#include "access_point.h"
#include <EspMQTTClient.h>

String topic_string = "";
String topic_string_callback = "";
String mqtt_user = "";
String mqtt_password = "";
String nome_dispositivo = "";

String ssid = "";
String password = "";
String broker = "broker.hivemq.com";
int port = 1883;

uint64_t chipid=ESP.getEfuseMac();
uint16_t chip = (uint16_t)(chipid>>32);
String client_id = (String)chip; 


EspMQTTClient client(
      ssid.c_str(),
      password.c_str(),
      broker.c_str(),
      "",
      "",
      client_id.c_str(),
      port);
 

void read_data();


void read_data()
{
  File ssid_file = SPIFFS.open(F("/ssid.txt"), "r");
  ssid = ssid_file.readString();
  ssid_file.close();
  File pssw_file = SPIFFS.open(F("/pssw.txt"), "r");
  password = pssw_file.readString();
  pssw_file.close();
  File broker_file = SPIFFS.open(F("/broker.txt"), "r");
  broker = broker_file.readString();
  broker_file.close();
  File port_file = SPIFFS.open(F("/port.txt"), "r");
  port = (port_file.readString()).toInt();
  port_file.close();
  File mqttpass_file = SPIFFS.open(F("/mqttpassword.txt"), "r");
  mqtt_password = mqttpass_file.readString();
  mqttpass_file.close();
  File mqttus_file = SPIFFS.open(F("/mqttuser.txt"), "r");
  mqtt_user = mqttus_file.readString();
  mqttus_file.close();
  File nome_file = SPIFFS.open(F("/nome.txt"), "r");
  nome_dispositivo = nome_file.readString();
  nome_file.close();

  topic_string = (mqtt_user + "/" + mqtt_password + "/wifilamp/" + nome_dispositivo);
  topic_string_callback = (mqtt_user + "/" + mqtt_password + "/wifilamp/callback/" + nome_dispositivo);

  client.enableLastWillMessage("topic_string_callback", "poff"); 
}


void onConnectionEstablished(){
  client.subscribe(topic_string, [](const String &payload){

    if (payload == "on") {
      digitalWrite(LED_BUILTIN, HIGH);
      client.publish(topic_string_callback, "on");
    }
    else if (payload == "off") {
       digitalWrite(LED_BUILTIN, LOW); 
      client.publish(topic_string_callback, "off");
    }
    else if (payload == "rainbow") {
      
      client.publish(topic_string_callback, "rainbow");
    } });

  client.publish(topic_string_callback, "pon", true);
}


#endif
