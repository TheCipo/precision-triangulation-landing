#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define MYADDRESS 10
#define ND -1
#define DATATIMES 21

// EEPROM addresses
#define EPR_Ax 0
#define EPR_Ay 4
#define EPR_Bx 8
#define EPR_By 12
#define EPR_Cx 16
#define EPR_Cy 20
#define EPR_Max 24

// Pin definitions
const int trigPins[3] = {2, 4, 6};
const int echoPins[3] = {3, 5, 7};

// Global variables (shared)
extern float Ax, Ay, Bx, By, Cx, Cy;
extern int MAXdistance;
extern float Dx, Dy;
extern bool error;

#endif