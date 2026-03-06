#include "comunication.h"
#include "config.h"
#include <Wire.h>

void I2C_DataSend() {
  Serial.print(MYADDRESS);
  for(int i = 0; i < 3; i++){
    Serial.println(" invia toSendDistances[");
    Serial.print(i);
    Serial.print("]:");
    Serial.print(toSendDistances[i]);
    Wire.write((byte *)&toSendDistances[i], sizeof(toSendDistances[i]));
  }
}

void maxReadGet(int setupInfo) {
  if (setupInfo >= sizeof(int)) {
    byte lowByte = Wire.read();
    byte highByte = Wire.read();
    MAXdistance = (highByte << 8) | lowByte; 
    bool maxKnown = true; // Sblocca il loop nel setup
  }
}