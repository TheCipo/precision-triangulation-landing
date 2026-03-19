#include "comunication.h"
#include "config.h"
#include <Wire.h>

void I2C_DataSend() {
  Serial.print(MYADDRESS);
  byte buffer[6];

  for (int i = 0; i < 3; i++) {
    int16_t value = (int16_t)toSendDistances[i];
    buffer[i * 2] = (uint8_t)(value & 0xFF);
    buffer[i * 2 + 1] = (uint8_t)((value >> 8) & 0xFF);
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
  if (setupInfo >= 2) {
    uint8_t low = (uint8_t)Wire.read();
    uint8_t high = (uint8_t)Wire.read();
    MAXdistance = (int)((uint16_t)low | ((uint16_t)high << 8));
    maxKnown = true;
  }
}
