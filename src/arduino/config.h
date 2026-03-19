#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define MYADDRESS 10
#define ND -9999
#define DATATIMES 21
#define SIMULATION 1 //set this to 0 to get data from ESP32, 1 to simulate data for MAXdistance = 80cm


// Pin definitions
const int trigPins[3] = {22, 24, 26};
const int echoPins[3] = {23, 25, 27};

// Global variables (shared)
extern int MAXdistance;
extern int MINdistance;
extern int toSendDistances[3];
extern bool maxKnown;

#endif