#ifndef pag2_h
#define pag2_h
#include "Arduino.h"

static const char PROGMEM pag2[] = R"rawliteral(

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
      p{
              font-size: 40pt;
              color:  #558c9f;;
      }
      p1{
              font-size: 30pt;
              color:  #67c9ed;
      }                  
            </style>

    </head>
    <body>
    <p>Configurazione riuscita.</p>
    
        <p1>Riavvio...</p1>
    </body>
</html>
)rawliteral";

#endif
