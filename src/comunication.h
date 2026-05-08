#ifndef COMUNICATION_H
#define COMUNICATION_H

#include "config.h"

String readLine(); //funzione per leggere una riga dalla seriale
void connectToWiFi(const char * ssid, const char * pwd); //funzione per connettersi al WiFi
void WiFiEvent(WiFiEvent_t event); //funzione per gestire gli eventi del WiFi

#endif