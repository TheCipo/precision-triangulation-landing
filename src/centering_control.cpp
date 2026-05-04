#include "centering_control.h"
#include "drone.h"
#include <Tello.h>

void degradedMode(int distance, int index) {
  Serial.println("Degraded mode!");
  int deg = 0;
  switch(index) {
    case 0:
      deg = 0;
      break;
    case 1:
      deg = 120;
      break;
    case 2:
      deg = 240;
      break;
    default:
      return; // se l'indice non è valido esce
  }
  //calcola il vettore di movimento in base alla distanza e all'angolo
  float cosDeg = cos(deg * PI / 180);
  float sinDeg = sin(deg * PI / 180);
  int x = distance * cosDeg;
  int y = distance * sinDeg;
  vectorToTello(-x, -y);
}