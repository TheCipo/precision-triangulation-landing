#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define MYADDRESS 10
#define ND -9999
#define DATATIMES 21


// Pin definitions
const int trigPins[3] = {2, 4, 6};
const int echoPins[3] = {3, 5, 7};

// Global variables (shared)
extern int MAXdistance;
extern bool error;
extern int toSendDistances[3];
extern bool maxKnown;

#endif