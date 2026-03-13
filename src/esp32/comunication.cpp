#include "comunication.h"
#include <Wire.h>

void getDistances() {
  Wire.requestFrom(RADARADRESS, 6); // Richiedi 6 byte (3 distanze da 2 byte ciascuna)
  for (int i = 0; i < 3; i++) {
    if (Wire.available() >= 2) {
      Distances[i] = Wire.read() | (Wire.read() << 8); // Combina i due byte in un int
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