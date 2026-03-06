#include "setup.h"
#include "config.h"
#include "comunication.h"
#include <Preferences.h>
#include <Wire.h>

void coordinateSensori() {
  Serial.print("R (raggio) = ");
  String input = readLine();
  float R = input.toFloat();

  Ax = 0; Ay = R;
  Bx = R * sqrt(3) / 2.0; By = -R / 2.0;
  Cx = -R * sqrt(3) / 2.0; Cy = -R / 2.0;

  Serial.print("MAXdistance = ");
  input = readLine();
  MAXdistance = input.toInt();
  Wire.beginTransmission(RADARADRESS);
  Wire.write((byte *)&MAXdistance, sizeof(MAXdistance));
  Wire.endTransmission();
}

void menuConfigurazione() {
  Serial.println("Coordinate attuali:");
  Serial.print("A("); Serial.print(Ax); Serial.print(","); Serial.print(Ay); Serial.println(")");
  Serial.println("1: Modifica Provvisoria, 2: Salva in EEPROM, 0: Esci");
  
  String input = readLine();
  int scelta = input.toInt();

  if (scelta == 1 || scelta == 2) {
    coordinateSensori();
    if (scelta == 2) {
      storage.putFloat("Ax", Ax);
      storage.putFloat("Ay", Ay);
      storage.putFloat("Bx", Bx);
      storage.putFloat("By", By);
      storage.putFloat("Cx", Cx);
      storage.putFloat("Cy", Cy);
      storage.putInt("MAXdistance", MAXdistance);
      Serial.println("Salvato!");
    }
  }
}