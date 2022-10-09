# LampApp
A pretty remotely controlled lamp.
🇮🇹/🇬🇧

LampApp è un progetto open-source e open_hardware dedicato ai maker ma non solo.
Si compone di una app e di un dispositivo(per adesso).

Tramite l'app è possibile pilotare tutti i dispositivi che avrai installato in casa tua tramite il nome che gli avrai dato.
Questo sistema sfrutta il protocollo MQTT ma per ora senza certificati quindi assicurati di non appoggiarti a reti pubbliche e non usare password
che usi altrove o comuni!

Per aggiungere un dispositivo nella tua app ti basterà andare alla voce `MENU` e poi ad `ADD` e inserire il nome del dispositivo che intendi utilizzare.
Fai attenzione alle maiuscole!
Per rimuoverlo usa il pulsante `REMOVE`


Per utilizzare l'app non sei costretto ad utilizzare il dispositivo ufficiale, puoi usare qualsiasi cosa sia in grado di sfruttare il protocollo MQTT.
Se decidi di creare il tuo dispositivo ci sono alcune cose che devi sapere.

L'app comunica tramite due topic principali:

`your_user/your-password/wifilamp/device_name`

Questo è il topic che l'app usa per comunicare col dispositivo.

`your_user/your-password/wifilamp/callback/device_name`

Questo invece è il topic attraverso il quale il dispositivo risponde all app.

I messaggi che invia la nostra app sono semplici stringhe di testo:

`on` quando premi sulla lampada quando è spenta.

`off` quando premi sulla lampada quando è accesa.

`rainbow` quando premi il pulsante `RAINBOW`.

`255,255,255,255` quando scegli un colore(rosso,verde,blu,luminosità).

Una volta che la nostra app ha inviato uno di questi messaggi si aspetta una risposta, ecco un esempio per ogni tipo di comando:


`your_user/your-password/wifilamp/device_name/on`

`your_user/your-password/wifilamp/callback/device_name/on`


`your_user/your-password/wifilamp/device_name/off`

`your_user/your-password/wifilamp/callback/device_name/off`


`your_user/your-password/wifilamp/device_name/rainbow`

`your_user/your-password/wifilamp/callback/device_name/rainbow`


`your_user/your-password/wifilamp/device_name/255,255,255,255`

`your_user/your-password/wifilamp/callback/device_name/ok`



[Download for Android](https://play.google.com/store/apps/details?id=com.lampapp.lampapp&hl=it&gl=IT) 

iOS: coming soon 




[How it works?(Come funziona?)](https://github.com/ChristianIannella/LampApp/blob/main/LampApp_1.2/README.md)

# Contribute
Please, feel free to contribute.


![Alt text](LampApp_1.2/images/Logo_100x100.png?raw=true "Title") 


# Author
(c) 2022 Iannella Christian
