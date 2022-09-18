#ifndef notfound_pag_h
#define notfound_pag_h
#include "Arduino.h"

static const char PROGMEM notfound_pag[] = R"rawliteral(
<!DOCTYPE xhtml>
<html>
  <head>
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">
  <title>LampApp</title>
    <style type="text/css">
      body{
              font-family: courier, courier-new, serif;
              text-align: center;
      }
      p1{
              font-size: 40pt;
              color:  #dc143c;
      }
      p{
              font-size: 30pt;
              color:  #67c9ed;
      }      
     
     
                        
      </style>

  </head>
    <body>
        <p1>Ops... sembra ci sia un problema!<p1>
    <p>Scansiona il codice QR</p>
    <p>oppure</p>
    <p>cerca: <a href=http:192.168.4.1</a></p>
  </body>
</html>
)rawliteral";

#endif
