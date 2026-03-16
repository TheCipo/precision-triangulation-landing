#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define MYADDRESS 10
#define ND -9999
#define DATATIMES 21


// Pin definitions
const int trigPins[3] = {22, 24, 26};
const int echoPins[3] = {23, 25, 27};

// Global variables (shared)
extern int MAXdistance;
extern int MINdistance;
extern int toSendDistances[3];
extern bool maxKnown;

#endif