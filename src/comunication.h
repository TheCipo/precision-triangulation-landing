#ifndef COMUNICATION_H
#define COMUNICATION_H

#include "config.h"
#include <WiFi.h>

String readLine(); //funzione per leggere una riga dalla seriale
void connectToWiFi(const char * ssid, const char * pwd); //funzione per connettersi al WiFi

#endif