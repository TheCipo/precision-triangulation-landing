#include <Wire.h>
#include <EEPROM.h>
#include "config.h"
#include "sensor.h"
#include "filter.h"
#include "comunication.h"
#include "setup.h"

// Inizializzazione variabili globali
float Ax, Ay, Bx, By, Cx, Cy;
int MAXdistance;
float Dx = ND, Dy = ND;
bool error = false;

void setup() {
  Serial.begin(9600);
  Wire.begin(MYADDRESS);
  Wire.onRequest(I2C_DataSend);

  for (int i = 0; i < 3; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  // Caricamento dati
  EEPROM.get(EPR_Ax, Ax); EEPROM.get(EPR_Ay, Ay);
  EEPROM.get(EPR_Bx, Bx); EEPROM.get(EPR_By, By);
  EEPROM.get(EPR_Cx, Cx); EEPROM.get(EPR_Cy, Cy);
  EEPROM.get(EPR_Max, MAXdistance);

  Serial.println("Invia un tasto entro 10s per il setup...");
  unsigned long startT = millis();
  while (millis() - startT < 10000) {
    if (Serial.available()) {
      menuConfigurazione();
      break;
    }
  }
}

void loop() {
  int multiDistances[3][DATATIMES];
  int finalDistances[3];
  error = false;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < DATATIMES; j++) {
      int d = readDistance(trigPins[i], echoPins[i]);
      if (d == ND) error = true;
      multiDistances[i][j] = d;
    }
  }

  if (!error) {
    for (int i = 0; i < 3; i++) {
      finalDistances[i] = getMedian(multiDistances[i], DATATIMES);
    }
    calculatePosition(finalDistances);
  } else {
    Dx = ND; Dy = ND;
  }

  // Nota: I2C_DataSend() viene chiamato automaticamente dall'interrupt Wire.onRequest
  // Qui lo usiamo solo per debug seriale se vuoi
  delay(2000);
}