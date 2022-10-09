#ifndef pag1_h
#define pag1_h
#include "Arduino.h"

static const char PROGMEM pag1[] = R"rawliteral(


<!DOCTYPE xhtml>
<html>
  <head>
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">
    <title>LampApp</title>
    <style type="text/css">
      p{
        font-family: courier, courier-new, serif;
        font-size: 40pt;
        color: #558c9f;
        text-align: center;
      }
      h3{
        font-family: courier, courier-new, serif;
        font-size: 20pt;
        color: #558c9f;
        text-align: center;
      }

      form{
        font-family: courier, courier-new, serif;
        font-size: 40pt;
        color: #67c9ed;
        text-align: center
      }
      input {
        border: 1px solid #558c9f;
        height: 35px;
        width: 200px;
      }
      input[type="submit"] {
        border: 1px solid;
        border-color: #558c9f;
        height: 50px;
        width: 200px;
        font-family: courier, courier-new, serif;
        font-size: 22pt;
                                color: #558c9f;
        padding: 0;
        border-radius: 5px;
        background: white;
      }
      </style>

  </head>
  <body>
    <p>Configura il tuo dispositivo.<p>
    <h3>Usa stesso User e Password che usi sull'app.<br>(minimo 12 caratteri).<h3>
    <br><br>
    <form method="post" action="/save" >
    Rete WiFI:<br><input name="Rete WiFi" type="text" size="16" value=""  ><br><br>
    <br><br>
    Password Wifi:<br><input name="Password WiFi" type="password" size="16" value="" ><br><br> 
    <br><br>
    Broker:<br><input name="Broker" type="text" size="16" value=""  ><br><br>
    <br><br>
    User:<br><input name="User" type="text" size="16" value="" ><br><br> 
    <br><br>
    Password:<br><input name="Password" type="password" size="16" value="" ><br><br>
    <br><br>
    Nome dispositivo:<br><input name="Nome dispositivo" type="text" size="16" value="" ><br><br>
    <br><br>
    <p><p>
    <input type="submit" name="clk_action" value="SALVA">
    </form>
    <form method="post" action="/info" >
    <input type="submit",  name="clk_info" value="INFO">  
    </form>
  </body>
</html>


)rawliteral";

#endif
