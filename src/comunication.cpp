#include "comunication.h"
#include <Wire.h>

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