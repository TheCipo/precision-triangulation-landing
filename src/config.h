#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

#define ND -9999
#define DATATIMES 21 //numero di campioni da memorizzare per ogni sensore (20 + 1 per il filtro)

// Pin definitions
const int trigPins[3] = {23, 19, 17}; //definizione dei pin dei trigger dei sensori
const int echoPins[3] = {22, 18, 16}; //definizione dei pin degli echo dei sensori

//global variables (shared)
extern Preferences storage; //inizializzazione del Preferences storage
extern float Ax, Ay, Bx, By, Cx, Cy; //inizializzazione delle coordinate dei sensori
extern float Dx, Dy; //inizializzazione delle coordinate del drone
extern int MAXdistance; //inizializzazione della distanza massima misurabile dai sensori
extern int MINdistance; //inizializzazione della distanza minima misurabile dai sensori
extern int Distances[3]; //inizializzazione dell'array per memorizzare le distanze misurate e pulite dei sensori
const int MAXtimeout = 15000; // Maximum time to wait for echo (in microseconds)

#endif