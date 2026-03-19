#include "comunication.h"
#include "config.h"
#include <Wire.h>

void I2C_DataSend() {
  Serial.print(MYADDRESS);
  byte buffer[6];
  for (int i = 0; i < 3; i++) {
    buffer[i * 2] = lowByte(toSendDistances[i]);  // Byte basso
    buffer[i * 2 + 1] = highByte(toSendDistances[i]); // Byte alto
  }
  Wire.write(buffer, 6);
  
  for(int i = 0; i < 3; i++){
    Serial.println(" invia toSendDistances[");
    Serial.print(i);
    Serial.print("]:");
    Serial.print(toSendDistances[i]);
  }
}

void maxReadGet(int setupInfo) {
  if (setupInfo >= sizeof(int)) {
    byte lowByte = Wire.read();
    byte highByte = Wire.read();
    MAXdistance = (highByte << 8) | lowByte; 
    maxKnown = true;
  }
}
