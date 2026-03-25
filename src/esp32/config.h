#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

#define MYADDRESS 0x01
#define RADARADRESS 0x10
#define ND -9999
#define MAXERRORS 10

//global variables
extern Preferences storage;
extern float Ax, Ay, Bx, By, Cx, Cy;
extern float Dx, Dy;
extern int MAXdistance;
extern int Distances[3];
extern int errorCount;

#endif