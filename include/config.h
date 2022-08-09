#ifndef config_h
#define config_h

#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"
#include "access_point.h"
#include "data.h"

void check_data();

void check_data()
{

  SPIFFS.begin();
  File ssid_file = SPIFFS.open(F("/ssid.txt"), "r");
  
  if (ssid_file)
  {
    
    if (ssid_file.readString() != "")
    {
      ssid_file.close();
      read_data();
    }
    else 
  {
    ssid_file.close();
    create_ap();
  }
  }
}

#endif
