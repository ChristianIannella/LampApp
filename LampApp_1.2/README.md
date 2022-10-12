
# HOW APP WORKS


![Alt text](screenshots/home_screen.png?raw=true "Title") 


Quest'app utilizza la libreria [client Python MQTT di Eclipse Paho](https://pypi.org/project/paho-mqtt/).


Per aggiungere un dispositivo nella tua app ti basterà andare alla voce `MENU` e poi ad `ADD` e inserire il nome del dispositivo che intendi utilizzare.
Fai attenzione alle maiuscole!
Per rimuoverlo usa il pulsante `REMOVE`


![Alt text](screenshots/menu_screen.png?raw=true "Title") 


Per utilizzare l'app non sei costretto ad utilizzare il dispositivo ufficiale, puoi usare qualsiasi cosa sia in grado di sfruttare il protocollo MQTT.
Se decidi di creare il tuo dispositivo ci sono alcune cose che devi sapere.

L'app comunica tramite due topic principali:

`your_user/your-password/wifilamp/device_name` Questo è il topic che l'app usa per comunicare col dispositivo.


`your_user/your-password/wifilamp/callback/device_name` Questo invece è il topic attraverso il quale il dispositivo risponde all app.



I messaggi che invia la nostra app sono semplici stringhe di testo:


* `on` quando premi sulla lampada quando è spenta.

* `off` quando premi sulla lampada quando è accesa.

* `rainbow` quando premi il pulsante `RAINBOW`.

* `255,255,255,255` quando scegli un colore(rosso,verde,blu,luminosità).


Una volta che la nostra app ha inviato uno di questi messaggi si aspetta una risposta, ecco un esempio per ogni tipo di comando:


Messaggio: `your_user/your-password/wifilamp/device_name/on`

Risposta: `your_user/your-password/wifilamp/callback/device_name/on`


Messaggio: `your_user/your-password/wifilamp/device_name/off`

Risposta: `your_user/your-password/wifilamp/callback/device_name/off`


Messaggio: `your_user/your-password/wifilamp/device_name/rainbow`

Risposta: `your_user/your-password/wifilamp/callback/device_name/rainbow`


Messaggio: `your_user/your-password/wifilamp/device_name/255,255,255,255`

Risposta: `your_user/your-password/wifilamp/callback/device_name/ok`

Ma come faccio a sapere se il dispositivo è collegato e funzionante?

Quando il dispositivo è alimentato e connesso alla rete, pubblica in automatico un messaggio sul topic di callback con contenuto `pon` che sta per 
power on, questo messaggio farà cambiare l'icona della spina in basso a sinistra ad indicare che il dispositivo è collegato. 
Lo stesso succedera anche se il dispositivo pubblica il messaggiop `on`, in questo caso aprendo l'app troveremo l'icona della spina cambiata e la lampada accesa.

Quando il dispositivo viene scollegato, il broker pubblicherà in automatico un messaggio di testamento con il contenuto `poff` che sta per power off, adesso l'iconna della spina sarà barrata e se provassimo ad accendere la lampada tramite app ci appararirà un messaggio che ci dice che il dispositivo non è collegato.

Tutti i messaggi di callback ovvero quelli dai dispositivi verso l'app dovranno avere il flag diretain impostato su true.


Il pulsante  `LOG` mostra un elenco di tutti i messaggi in entrata e in uscita.


