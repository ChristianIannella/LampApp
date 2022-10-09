#ifndef data_h
#define data_h

#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"

extern String topic_string;
extern String topic_string_callback;
extern String mqtt_user;
extern String mqtt_password;
extern String nome_dispositivo;
extern String ssid;
extern String password;
extern String broker;

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

}

#endif
