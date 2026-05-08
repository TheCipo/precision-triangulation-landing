#include "centering_control.h"
#include "drone.h"
#include <Tello.h>

void degradedMode(int distance, int index) {
  Serial.println("Degraded mode!");
  int deg = 120 * index; // calcola l'angolo in base all'indice del sensore
  //calcola il vettore di movimento in base alla distanza e all'angolo
  float cosDeg = cos(deg * PI / 180);
  float sinDeg = sin(deg * PI / 180);
  int x = distance * cosDeg;
  int y = distance * sinDeg;
  vectorToTello(-x, -y);
}