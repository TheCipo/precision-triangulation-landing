#include <Wire.h>
#include "config.h"
#include "sensor.h"
#include "filter.h"
#include "comunication.h"

// Inizializzazione variabili globali
int MAXdistance = ND;
int MINdistance = ND;
bool error = false;
int toSendDistances[3] = {ND, ND, ND};
bool maxKnown = false;

void setup() {
  Serial.begin(9600);
  Wire.begin(MYADDRESS);
  Wire.onRequest(I2C_DataSend);

  for (int i = 0; i < 3; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  //no data from esp32
  MAXdistance = 80;
  MINdistance = 5;

  //data from ESP32
  /*Serial.println("Waiting for data from ESP32");
  Wire.onReceive(maxReadGet);
  while (!maxKnown){delay(10);Serial.print(".");}
	Serial.println("data arrived from ESP32 MAXdistance = "); 
	Serial.print(MAXdistance);
	Wire.onReceive(NULL);*/
}

void loop() {
  int multiDistances[3][DATATIMES];
  int finalDistances[3];

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < DATATIMES; j++) {
      int d = readDistance(trigPins[i], echoPins[i]);
      multiDistances[i][j] = d;
      delay(60);
    }
  }

  
  for (int i = 0; i < 3; i++) {
    finalDistances[i] = filter(multiDistances[i]);
  }
  for(int i = 0; i < 3; i++){
    toSendDistances[i] = finalDistances[i];
  }

  for(int i = 0; i < 3; i++){
     Serial.print(toSendDistances[i]);
     Serial.print(", ");
  }
  Serial.println("");
}