#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
#include "secrets.h"

#define ND -9999
#define DATATIMES 5 //numero di campioni da memorizzare per ogni sensore (20 + 1 per il filtro)
#define MAXERRORS 10 //numero massimo di errori consecutivi prima di entrare in modalità degradata

// Pin definitions
const int trigPins[3] = {23, 19, 17}; // trigger dei sensori (bianchi)
const int echoPins[3] = {22, 18, 16}; // echo dei sensori (verdi)

//definizione per il drone
inline const char * networkName = TELLO_NAME; //nome del drone da connettere, definito in secrets.h
inline const char * networkPswd = TELLO_PASSWORD; //password del drone, se c'è (di solito è vuota)

// Global variables (shared)
extern Preferences storage;
extern float Ax, Ay, Bx, By, Cx, Cy;
extern float Dx, Dy;
extern int MAXdistance;
extern int MINdistance;
extern float sensorAngle; //angolo di inclinazione del sensore
const int MAXtimeout = 18000; // Aumentato a 18000 us (~300 cm) per evitare falsi ND
extern int errorCount;
extern int padIndex;
extern int acceptedDistanceError;
extern int padVector[2];
extern bool connected;
extern bool landed;
extern int finalDistances[3];


#endif