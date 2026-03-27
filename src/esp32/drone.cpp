#include "drone.h"
#include <Tello.h>

Tello drone;

void vectorToTello(int x, int y){
  if (x > 0) { // se x è positivo, muovi a destra
    drone.right(x);
  } else if (x < 0) { // se x è negativo, muovi a sinistra
    drone.left(-x);
  }
  if (y > 0) { // se y è positivo, muovi in avanti
    drone.forward(y);
  } else if (y < 0) { // se y è negativo, muovi indietro
    drone.backward(-y);
  }
}

void resetTello() {
  //TODO: implementare reset del drone
}