#include "centering_control.h"
#include "drone.h"
#include "tello.h"
#include "config.h"

void degradedMode(int distance, int index) {
  Serial.println("==================================================");
  Serial.println("[MODALITA' DEGRADATA - 1 SENSORE]");
  
  if (index < 0 || index > 2) {
    Serial.println("Invalid sensor index");
    return;
  }
  
  // Array per gestire i dati dei sensori
  float sensorCoords[3][2] = {{Ax, Ay}, {Bx, By}, {Cx, Cy}};
  const char* sensorNames[3] = {"Sensor A", "Sensor B", "Sensor C"};
  
  Serial.print("Sensore attivo: ");
  Serial.print(sensorNames[index]);
  Serial.print(" | Distanza letta: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  float sx = sensorCoords[index][0];
  float sy = sensorCoords[index][1];
  float sensorDist = sqrt(sx * sx + sy * sy);
  if (sensorDist <= 0.0f) {
    Serial.println("Invalid sensor coordinates");
    return;
  }
  
  // Calcola la direzione dalla posizione del sensore verso l'origine
  float dirX = -sx / sensorDist;
  float dirY = -sy / sensorDist;
  
  // Aggiorna anche le globali Dx, Dy per riferimento nel log pre-movimento
  Dx = sx + distance * dirX;
  Dy = sy + distance * dirY;
  
  Serial.print("[RIFERIMENTO] Coordinate stimate drone: (X = ");
  Serial.print(Dx);
  Serial.print(" cm, Y = ");
  Serial.print(Dy);
  Serial.println(" cm)");
  Serial.println("==================================================");
  
  vectorToTello((int)round(-Dx), (int)round(-Dy));
}

void stdMode(int x, int y) {
  int distance = (int)round(sqrt(x * x + y * y));
  Serial.println("==================================================");
  Serial.print("[CONTROLLO CENTRAGGIO] Coordinate drone: (X = ");
  Serial.print(Dx);
  Serial.print(" cm, Y = ");
  Serial.print(Dy);
  Serial.println(" cm)");
  Serial.print("Distanza dal centro (0,0): ");
  Serial.print(distance);
  Serial.print(" cm (Soglia accettata: ");
  Serial.print(acceptedDistanceError);
  Serial.println(" cm)");
  Serial.println("==================================================");

  if (distance <= acceptedDistanceError) {
    Serial.println(">>> DRONE CENTRATO! Eseguo atterraggio... <<<");
    drone.land();
    landed = true;
  } else {
    Serial.println(">>> Drone NON centrato, preparo manovra di correzione <<<");
    vectorToTello(-x, -y);
    landed = false; 
  }
}