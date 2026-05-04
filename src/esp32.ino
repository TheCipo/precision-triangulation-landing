#include <Wire.h>
#include "config.h"
#include "positioning.h"
#include "drone.h"
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
bool padVector[2]; //vettore per tracciare dove si trova il pad (0: x, 1: y)

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
}

void loop(){
  //pulizia dei dati precedenti
  multiDistances[3][DATATIMES] = {ND};
  int finalDistances[3] = {ND, ND, ND};
  if(errorCount > MAXERRORS) { // se ci sono troppi errori ricomincia da capo
    resetTello();
    errorCount = 0;
    return;
  }

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

  if(Distances[0] ==  ND || Distances[1] == ND || Distances[2] == ND) { //controlla se tutte le distanze sono valide
    errorCount++; //incrementa il contatore di errori
    int index = -1; 
    for(int i = 0; i < 3; i++) { //cerca la prima distanza valida
      if(Distances[i] != ND) {
        index = i;
        break;
      }
    }

    if(index >= 0 && index <= 2){ //se c'è alemno una distanza valida fa un ciclo in Degraded Mode
      degradedMode(Distances[index], index);
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
  }
}