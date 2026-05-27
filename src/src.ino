#include <Wire.h>
#include <WiFi.h>
#include "config.h"
#include "positioning.h"
#include "drone.h"
#include "comunication.h"
#include "setup.h"
#include "centering_control.h"
#include "sensor.h"
#include "filter.h"
#include "secrets.h"

//inizializzazione variabili globali
Preferences storage; //archiviazione dati di configurazione
float Ax, Ay, Bx, By, Cx, Cy; //coordinare dei sensori
float Dx, Dy; //coordinare del drone
int MAXdistance = 80; //distanza massima misurabile dai sensori
int MINdistance = 2; //distanza minima misurabile dai sensori
int multiDistances[3][DATATIMES]; //distanze raccolte da filtrare
int finalDistances[3]; //distanze finali dopo il filtro
bool error = false; //variabile per segnalare errori di misurazione
int errorCount = 0; //contatore di errori consecutivi
int padIndex = 0; //indice del pad attualmente tracciato (0, 1 o 2)
int padVector[2]; //vettore per tracciare dove si trova il pad (0: x, 1: y)
bool connected = false; //variabile per segnalare se il drone è connesso al WiFi
bool landed = false; //variabile per segnalare se il drone è atterrato
int acceptedDistanceError = 5; //soglia per accettare le misurazioni delle distanze come valide (in cm)
float cosTheta = 0.86602540378; //coseno dell'angolo dei sensori rispetto al terreno (in gradi)

void setup(){
  storage.begin("landing", false); //inizializzazione del Preferences storage
  Wire.begin(); //inizializzazione del bus I2C
  Serial.begin(9600); //inizializzazione della seriale
  //menu di configurazione
  Serial.println("Invia un tasto entro 10s per il setup...");
  unsigned long startT = millis();
  loadSetup();
  while (millis() - startT < 10000) {
    if (Serial.available()) {
      menuConfigurazione(); //apertura del menu di configurazione se viene ricevuto un input entro 10 secondi
      break;
    }
  }
  storage.end(); //chiusura del Preferences storage

  for(int i = 0; i < 3; i++){ //configurazione dei pin dei sensori
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  connectToWiFi(networkName, networkPswd); //connessione al drone tramite WiFi
  resetTello(); //reset del drone per portarlo alla posizione di partenza
}

void loop(){
  //pulizia dei dati precedenti
  int finalDistances[3] = {ND, ND, ND};
  if(errorCount > MAXERRORS) { // se ci sono troppi errori ricomincia da capo
    resetTello();
    errorCount = 0;
    return;
  }

  if(!landed){ //se il drone non è atterrato continua a cercare di centrarlo
    //raccolta dei dati dai sensori
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < DATATIMES; j++) {
        int d = readDistance(trigPins[i], echoPins[i]);
        multiDistances[i][j] = d;
        delay(60);
      }
    }
    //filtro dei dati raccolti
    for (int i = 0; i < 3; i++) {
      finalDistances[i] = filter(multiDistances[i]);
    }
    //stampa delle distanze filtrate per debugging
    for(int i = 0; i < 3; i++){
      Serial.print(finalDistances[i]);
      Serial.print(", ");
    }

    if(finalDistances[0] == ND || finalDistances[1] == ND || finalDistances[2] == ND) { //controlla se tutte le distanze sono valide
      errorCount++; //incrementa il contatore di errori
      int index = -1; 
      for(int i = 0; i < 3; i++) { //cerca la prima distanza valida
        if(finalDistances[i] != ND) {
          index = i;
          break;
        }
      }

      if(index >= 0 && index <= 2){ //se c'è almeno una distanza valida fa un ciclo in Degraded Mode
        degradedMode(finalDistances[index], index);
      }else{ //se non c'è nessuna distanza valida ricomincia
        Serial.print("impossibile calcolare la posizione errore: ");
        Serial.println(errorCount);
        return;
      }

    }else{ // se tutte le distanze sono valide calcola la posizione
      errorCount = 0; //reset del contatore errori
      calculatePosition(finalDistances); //calcolo della posizione del drone
      //stampa della posizione calcolata per debugging
      Serial.print("Posizione: (");
      Serial.print(Dx);
      Serial.print(", ");
      Serial.print(Dy);
      Serial.println(")");
      stdMode(Dx, Dy); //controllo per centrare il drone
    }
  }
}