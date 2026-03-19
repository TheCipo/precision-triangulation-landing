#include "comunication.h"
#include <Wire.h>

void getDistances() {
  Wire.requestFrom(RADARADRESS, 6); // Richiedi 6 byte (3 distanze da 2 byte ciascuna)

  for (int i = 0; i < 3; i++) {
    if (Wire.available() >= 2) {
      uint8_t low = Wire.read();
      uint8_t high = Wire.read();
      int16_t value = (int16_t)((uint16_t)low | ((uint16_t)high << 8));
      Distances[i] = (int)value;
      } else {
      Distances[i] = ND; // Se non ci sono abbastanza byte, assegna ND
    }
  }
}

String readLine(){
  String datoricevuto = "";
  while (Serial.available()) Serial.read(); 
  while (!Serial.available());
  delay(10);
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') break;
    datoricevuto += c;
  }
  return datoricevuto;
}