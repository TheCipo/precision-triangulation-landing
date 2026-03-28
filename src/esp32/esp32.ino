#include <Wire.h>
#include "config.h"
#include "comunication.h"
#include "positioning.h"
#include "drone.h"
#include "setup.h"
#include "centering_control.h"
#include "../secrets.h"

//inizializzazione variabili globali
Preferences storage;
float Ax, Ay, Bx, By, Cx, Cy;
float Dx, Dy;
int MAXdistance;
int Distances[3];
int errorCount = 0;

void setup(){
  storage.begin("landing", false);
  Wire.begin(MYADDRESS);
  Serial.begin(9600);
  Serial.println("Invia un tasto entro 10s per il setup...");
  unsigned long startT = millis();
  loadSetup();
  while (millis() - startT < 10000) {
    if (Serial.available()) {
      menuConfigurazione();
      break;
    }
  }
  sendMAXdistance();
  storage.end();
}

void loop(){
  if(errorCount > MAXERRORS) { // se ci sono troppi errori ricomincia da capo
    resetTello();
    errorCount = 0;
    return;
  }
  getDistances(); //si fa mandare le distanze da Arduino
  if(Distances[0] ==  ND || Distances[1] == ND || Distances[2] == ND) { //controlla se tutte le distanze sono valide
    errorCount++;
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
    calculatePosition(Distances); //calcolo della posizione
    Serial.print("Posizione: (");
    Serial.print(Dx);
    Serial.print(", ");
    Serial.print(Dy);
    Serial.println(")");
  }
}