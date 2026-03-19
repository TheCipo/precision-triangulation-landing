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
  Wire.write((uint8_t)(MAXdistance & 0xFF));
  Wire.write((uint8_t)((MAXdistance >> 8) & 0xFF));
  Wire.endTransmission();
}

void menuConfigurazione() {
  Serial.println("Entrato in modalità setup.");
  Serial.println("Coordinate dei Sensori:");
  Serial.print("Ax = "); Serial.print(Ax); Serial.print(" Ay = "); Serial.println(Ay);
  Serial.print("Bx = "); Serial.print(Bx); Serial.print(" By = "); Serial.println(By);
  Serial.print("Cx = "); Serial.print(Cx); Serial.print(" Cy = "); Serial.println(Cy);
  Serial.print("Distanza Massima = "); Serial.println(MAXdistance);
  Serial.println("Inserisci 0 per uscire, 1 per sceglierli provvisoriamente, 2 per modificare la EEPROM: ");
  
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