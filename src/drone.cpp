#include "drone.h"
#include <Tello.h>
#include "config.h"

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
  drone.mon(); // abilita riconoscimento pad
  delay(7000);
  drone.mdirection(2); // abilita sensori frontale che verticale
  delay(5000);
  drone.takeoff(); // decolla
  delay(6000);
  drone.flyto(0, 0, 50, 50, STARTINGPAD); // riconosce pad 1 e si alza 50 cm e velocità 50cm/s  (x y z velocità pad)
  delay(5000);
  vectorToTello(-padVector[0], -padVector[1]); // muove il drone in base alla posizione del pad
  delay(5000);
}

