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

void setup(){
  storage.begin("landing", false);
  Wire.begin();
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
  if (MAXdistance > 0) {
    sendMAXdistance();
  }
  storage.end();
}

void loop(){
    getDistances();
    calculatePosition(Distances);
    Serial.print("Posizione: (");
    Serial.print(Dx);
    Serial.print(", ");
    Serial.print(Dy);
    Serial.println(")");
}