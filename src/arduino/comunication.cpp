#include "comunication.h"
#include "config.h"
#include <Wire.h>

void I2C_DataSend() {
  Serial.print(MYADDRESS);
  Serial.print(" invio X: "); Serial.print(Dx);
  Wire.write((byte *)&Dx, sizeof(Dx));
  Serial.print(" Y: "); Serial.println(Dy);
  Wire.write((byte *)&Dy, sizeof(Dy));
}

String readLine() {
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