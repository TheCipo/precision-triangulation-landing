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
Preferences storage;
float Ax, Ay, Bx, By, Cx, Cy;
float Dx, Dy;
int MAXdistance = 80;
int MINdistance = 2;
int Distances[3];
int multiDistances[3][DATATIMES];
int finalDistances[3];
bool error = false;

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
  storage.end();
}

void loop(){
  multiDistances[3][DATATIMES] = {ND};
  finalDistances[3] = {ND, ND, ND};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < DATATIMES; j++) {
      int d = readDistance(trigPins[i], echoPins[i]);
      multiDistances[i][j] = d;
      delay(60);
    }
  }
  for (int i = 0; i < 3; i++) {
    finalDistances[i] = filter(multiDistances[i]);
  }
  for(int i = 0; i < 3; i++){
     Serial.print(toSendDistances[i]);
     Serial.print(", ");
  }

  calculatePosition(finalDistances);
  Serial.print("Posizione: (");
  Serial.print(Dx);
  Serial.print(", ");
  Serial.print(Dy);
  Serial.println(")");
}