#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
#include "secrets.h"

#define ND -9999
#define DATATIMES 5 //numero di campioni da memorizzare per ogni sensore (20 + 1 per il filtro)
#define MAXERRORS 10 //numero massimo di errori consecutivi prima di entrare in modalità degradata

// Pin definitions
const int trigPins[3] = {23, 19, 17}; //definizione dei pin dei trigger dei sensori
const int echoPins[3] = {22, 18, 16}; //definizione dei pin degli echo dei sensori

//definizione per il drone
inline const char * networkName = TELLO_NAME; //nome del drone da connettere, definito in secrets.h
inline const char * networkPswd = TELLO_PASSWORD; //password del drone, se c'è (di solito è vuota)

//global variables (shared)
extern Preferences storage; //inizializzazione del Preferences storage
extern float Ax, Ay, Bx, By, Cx, Cy; //inizializzazione delle coordinate dei sensori
extern float Dx, Dy; //inizializzazione delle coordinate del drone
extern int MAXdistance; //inizializzazione della distanza massima misurabile dai sensori
extern int MINdistance; //inizializzazione della distanza minima misurabile dai sensori
const int MAXtimeout = 15000; // Maximum time to wait for echo (in microseconds)
extern int errorCount; // Counter for consecutive errors
extern int padIndex; // Index of the pad currently being tracked (0, 1, or 2)
extern int acceptedDistanceError; // Threshold for accepting distance measurements as valid (in cm)
extern int padVector[2]; // Vector to track where the pad is located (0: x, 1: y)
extern bool connected; // Variable to indicate if the drone is connected to WiFi
extern bool landed; // Variable to indicate if the drone has landed
extern int finalDistances[3]; // Distances collected

#endif