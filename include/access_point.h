#ifndef access_point_h
#define access_point_h

#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "SPIFFS.h"
#include "pag1.h"
#include "pag2.h"
#include "pag_info.h"
#include "notfound_pag.h"

const char *ap_ssid = "WiFi Lamp";
const char *ap_password = "12345678";

WebServer server(80);

extern bool ap_mode;

void create_ap();
void handleconf();
void handleNotFound();
void handlesave();

void handleconf()
{
  server.send(200, "text/html", pag1);
}

void handleinfo()
{
  server.send(200, "text/html", pag_info);
}

void handleNotFound()
{
  server.send(404, "text/html", notfound_pag);
}

void handlesave()
{
  String str = "";

  if (server.args() > 0)
  {
    for (uint8_t i = 0; i < 6; i++)
    {
      str += server.argName(i) + " = " + server.arg(i) + "\r\n";

      if (i == 0)
      {
        File ssid_file = SPIFFS.open(F("/ssid.txt"), "w");
        ssid_file.print(server.arg(i));
        ssid_file.close();
      }
      else if (i == 1)
      {
        File pssw_file = SPIFFS.open(F("/pssw.txt"), "w");
        pssw_file.print(server.arg(i));
        pssw_file.close();
      }

      else if (i == 2)
      {
        File broker_file = SPIFFS.open(F("/broker.txt"), "w");
        broker_file.print(server.arg(i));
        broker_file.close();
      }
      else if (i == 3)
      {
        File mqttus_file = SPIFFS.open(F("/mqttuser.txt"), "w");
        mqttus_file.print(server.arg(i));
        mqttus_file.close();
      }
      else if (i == 4)
      {
        File mqttpas_file = SPIFFS.open(F("/mqttpassword.txt"), "w");
        mqttpas_file.print(server.arg(i));
        mqttpas_file.close();
      }      
      else if (i == 5)
      {
        File nome_file = SPIFFS.open(F("/nome.txt"), "w");
        nome_file.print(server.arg(i));
        nome_file.close();
      }
    }
  }
  server.send(200, "text/html", pag2);
  delay(1000);
  ESP.restart();
}

void create_ap()
{
 
  ap_mode = true;
  WiFi.mode(WIFI_AP_STA);
 
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress IP = WiFi.softAPIP();
  // ip: 192.168.4.1
  server.on("/", handleconf );
  server.on("/save", handlesave);
  server.on("/info", handleinfo);
  server.onNotFound(handleNotFound);

  server.begin();
 
}

#endif
