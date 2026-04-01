#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

#define ND -9999
#define DATATIMES 21

// Pin definitions
#if defined(ESP32_DEV_BOARD)
    const int trigPins[3] = {2, 4, 6};
    const int echoPins[3] = {3, 5, 7};
#else
    #error "Unsupported board"
#endif

//global variables (shared)
extern Preferences storage;
extern float Ax, Ay, Bx, By, Cx, Cy;
extern float Dx, Dy;
extern int MAXdistance;
extern int MINdistance;
extern int Distances[3];
const int MAXtimeout = 15000; // Maximum time to wait for echo (in microseconds)

#endif