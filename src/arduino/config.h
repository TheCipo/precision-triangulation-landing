#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define MYADDRESS 0x10
#define ND -9999
#define DATATIMES 21
#define SIMULATION 1 //set this to 0 to get data from ESP32, 1 to simulate data for MAXdistance = 80cm


// Pin definitions
#if defined(ARDUINO_AVR_UNO)
    const int trigPins[3] = {2, 4, 6};
    const int echoPins[3] = {3, 5, 7};
#elif defined(ARDUINO_AVR_MEGA2560)
    const int trigPins[3] = {22, 24, 26};
    const int echoPins[3] = {23, 25, 27};
#else
    #error "Unsupported board"
#endif

// Global variables (shared)
extern int MAXdistance;
extern int MINdistance;
extern int toSendDistances[3];
extern volatile bool maxKnown;
const int MAXtimeout = 15000; // Maximum time to wait for echo (in microseconds)

#endif