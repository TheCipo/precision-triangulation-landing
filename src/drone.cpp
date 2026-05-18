#include "drone.h"
#include "tello.h"
#include "config.h"

Tello drone;

void vectorToTello(int x, int y){
  if (x < -9000){
    return;
  }
  
  if (x > 0) { // se x è positivo, muovi a destra
    drone.right(x);
  } else if (x < 0) { // se x è negativo, muovi a sinistra
    drone.left(-x);
  }
  delay(5000); // aggiungi un piccolo ritardo tra i comandi per evitare sovraccarichi
  if (y > 0) { // se y è positivo, muovi in avanti
    drone.forward(y);
  } else if (y < 0) { // se y è negativo, muovi indietro
    drone.back(-y);
  }
}

void resetTello() {
  if (!connected) {
    Serial.println("Drone non connesso, impossibile decollare.");
    return;
  }
  drone.mon(); // abilita riconoscimento pad
  delay(7000);
  drone.mdirection(2); // abilita sensori frontale che verticale
  delay(5000);
  drone.takeoff(); // decolla
  delay(6000);
  drone.flyto(0, 0, 50, 50, padIndex); // riconosce pad 1 e si alza 50 cm e velocità 50cm/s  (x y z velocità pad)
  delay(5000);
  drone.forward(50); // muove in avanti di 50 cm per essere sicuro di essere sopra il pad
  //vectorToTello(-padVector[0], -padVector[1]); // muove il drone in base alla posizione del pad
  delay(5000);
}

