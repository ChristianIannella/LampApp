
# LampApp Devices


![This is an image](https://github.com/ChristianIannella/LampApp/blob/main/LampApp%20device/file/pcb.png)



Si tratta di una scheda di sviluppo basata su ESP32 WROOM. Compatibile sia con Arduino che MicroPythn. 
La scheda all' acquisto arriverà con il firmware gia installato. Nel caso si volesse scrivere il proprio basterà collegarla tramite USB-C al pc e caricare il nuovo firmware come una normale scheda di sviluppo ESP32.

Nel caso state usando PlatformIO usate queste ipostazioni:

'platform = espressif32
board = featheresp32
framework = arduino'

Nel caso usaste Arduino IDE selezionate la scheda Adafruit ?????????

Caratteristiche scheda:

  ESP32 WROOM
  
    - 240 MHz dual core Tensilica LX6 microcontroller with 600 DMIPS
    - Integrated 520 KB SRAM
    - Integrated 802.11b/g/n HT40 Wi-Fi transceiver, baseband, stack and LWIP
    - Integrated dual mode Bluetooth (classic and BLE)
    - 4 MByte flash include in the WROOM32 module
    - On-board PCB antenna
  
  USB-C
    
    - 500 mA
    
  SLOT STRIP LED
    
    - WS2812
