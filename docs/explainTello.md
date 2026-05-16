# Spiegazione della libreria Tello
Tello.h Tello.cpp Author: akki

## Introduzione
Questa libreria implementa una semplice interfaccia UDP per il drone DJI Tello. Tutti i comandi sono inviati come stringhe al drone sull'indirizzo `192.168.10.1:8889` e la risposta viene letta dallo stesso socket UDP.

La libreria utilizza `WiFiUDP` per inviare i comandi e `string` per formare i payload.

## Inizializzazione
La prima cosa da fare è chiamare `Tello::init()`. Questa funzione:

- crea un nuovo oggetto `WiFiUDP`
- avvia il socket UDP su `COMMAND_PORT` (8889)
- imposta `isInitialised = true`
- invia il comando `command` al drone per passare in modalità SDK

Se il drone risponde con `ok`, la connessione è pronta. Notare che il codice attuale lascia comunque `isInitialised = true` anche se la risposta non è `ok`.

## Funzionamento di sendCommand
Ogni comando viene inviato tramite la funzione privata `Tello::sendCommand(string command)`:

    string Tello::sendCommand(string command)
    {
        if(!isInitialised)
            return "error";

        udpclient->beginPacket(TELLOIPADDRESS, COMMAND_PORT);
        udpclient->write((const unsigned char*)command.c_str(), command.length());
        udpclient->endPacket();

        char buffer[MAXBUFFSIZE] = {0};
        udpclient->parsePacket();
        int n = udpclient->read(buffer, MAXBUFFSIZE);
        if(n > 0)
        {
            buffer[n] = '\0';
        }
        printf("sendCommand: %s response=%s\n", command.c_str(), buffer);

        return string(buffer);
    }

Se la classe non è stata inizializzata (`isInitialised == false`), restituisce la stringa `error`.

## Comandi base
La libreria fornisce le seguenti funzioni di alto livello:

- `takeoff()` -> invia `takeoff`
- `land()` -> invia `land`
- `startVideoStream()` -> invia `streamon`
- `stopVideoStream()` -> invia `streamoff`
- `turnOff()` -> invia `emergency`

Ciascuna di queste funzioni controlla se la risposta è esattamente `ok` e restituisce `true` in quel caso.

## Comandi di movimento
Le funzioni di movimento inviano i comandi SDK standard con un parametro numerico:

- `up(int x)` -> `up x`
- `down(int x)` -> `down x`
- `left(int x)` -> `left x`
- `right(int x)` -> `right x`
- `forward(int x)` -> `forward x`
- `back(int x)` -> `back x`
- `rotate_clockwise(int deg)` -> `cw deg`
- `rotate_anticlockwise(int deg)` -> `ccw deg`

Esistono anche i comandi `flip`:

- `flip_front()` -> `flip f`
- `flip_right()` -> `flip r`
- `flip_left()` -> `flip l`
- `flip_back()` -> `flip b`

Il parametro `x` rappresenta la distanza in centimetri o i gradi di rotazione, a seconda del comando.

## Comandi avanzati
Sono implementati anche comandi più complessi:

- `setSpeed(int x)` -> `speed x`
  - accetta valori tra 10 e 100
- `sendRCcontrol(int a, int b, int c, int d)` -> `rc a b c d`
  - `a` = left/right (-100..100)
  - `b` = forward/backward (-100..100)
  - `c` = up/down (-100..100)
  - `d` = yaw (-100..100)
- `curve(int x1, int x2, int y1, int y2, int z1, int z2, int speed)` -> `curve x1 y1 z1 x2 y2 z2 speed`
- `go(int x, int y, int z, int speed)` -> `go x y z speed`
- `flyto(int x, int y, int z, int speed, int mid)` -> `go x y z speed m<mid>`
- `jump(int x, int y, int z, int speed, int yaw, int mid1, int mid2)` -> `go x y z speed yaw m<mid1> m<mid2>`

> Nota: l'implementazione del metodo `go` nella libreria non supporta direttamente il marker `mid` come mostrato nei commenti, mentre `flyto` e `jump` creano la stringa `m<mid>`.

## Query di stato
La libreria fornisce anche comandi di interrogazione del drone:

- `getSpeed()` -> `speed?`
- `getBattery()` -> `battery?`
- `getTime()` -> `time?`
- `getHeight()` -> `height?`
- `getTemperature()` -> `temp?`
- `getAltitude()` -> `attitude?`
- `getBaro()` -> `baro?`
- `getAcceleration()` -> `acceleration?`
- `getTof()` -> `tof?`
- `getWifiSnr()` -> `wifi?`

Ogni funzione invia il comando corrispondente e converte la risposta in intero con `atoi()`.

## Comandi di modalità e wifi
La libreria fornisce anche:

- `changeWifi(string ssid, string password)` -> `wifi <ssid> <password>`
- `mdirection(int x)` -> `mdirection x`
- `mon()` -> `mon`
- `moff()` -> `moff`

Questi ultimi comandi gestiscono la telemetria o la direzione del marker nel drone.

## Note importanti sul codice
- `sendCommand()` legge la risposta dal UDP senza timeout esplicito.
- `MAXBUFFSIZE` è impostato a 1472, che è la dimensione massima utile per UDP su IPv4.
- `check_range(int x, int min, int max)` restituisce `true` solo se `min < x < max`, quindi esclude gli estremi.
- Alcuni metodi commentati suggeriscono funzionalità non completamente implementate nel codice corrente.

## Esempio d'uso
Un tipico flusso di utilizzo in `setup()` potrebbe essere:

    Tello tello;
    tello.init();
    if (tello.takeoff()) {
        tello.forward(50);
        tello.rotate_clockwise(90);
        tello.land();
    }

In questo esempio il drone entra in modalità SDK, decolla, avanza di 50 cm, ruota di 90 gradi e atterra.

## Conclusione
La libreria fornisce un wrapper semplice dei comandi SDK Tello. Ogni metodo di movimento invia una stringa via UDP e verifica che la risposta sia `ok`. Le query di stato richiedono valori numerici, convertiti con `atoi()`.

