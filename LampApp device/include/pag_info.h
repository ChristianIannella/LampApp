#ifndef pag_info_h
#define pag_info_h
#include "Arduino.h"

static const char PROGMEM pag_info[] = R"rawliteral(


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
              color:  #558c9f;
      }    
        form{
        font-family: courier, courier-new, serif;
        font-size: 30pt;
        color: #67c9ed;
        text-align: center
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
    <p>LampApp</p>
    <p1>Di Iannella Christian.</p1>
    </form>
    <p>Per info visita: linktr.ee/chris_maker_ </p>
    <form method="post" action="/" >
    <input type="submit",  name="clk_info" value="INDIETRO">  
    </form>
  </body>
</html>
)rawliteral";

#endif
