#include "centering_control.h"
#include "drone.h"
#include "tello.h"
#include "config.h"

void degradedMode(int SensorDistance, int index) {
  Serial.println("Degraded mode!");
  
  if (index < 0 || index > 2) {
    Serial.println("Invalid sensor index");
    return;
  }
  
  // Array per gestire i dati dei sensori
  float sensorCoords[3][2] = {{Ax, Ay}, {Bx, By}, {Cx, Cy}};
  const char* sensorNames[3] = {"Sensor A", "Sensor B", "Sensor C"};
  
  Serial.println(sensorNames[index]);
  
  float sx = sensorCoords[index][0];
  float sy = sensorCoords[index][1];
  float sensorDist = sqrt(sx * sx + sy * sy);
  if (sensorDist <= 0.0f) {
    Serial.println("Invalid sensor coordinates");
    return;
  }

  float distance = SensorDistance * cosTheta; // Adjust distance for sensor angle
  
  // Calcola la direzione dalla posizione del sensore verso l'origine
  float dirX = -sx / sensorDist;
  float dirY = -sy / sensorDist;
  
  // Posizione stimata del drone sulla semiretta sensore->origine
  float x = sx + distance * dirX;
  float y = sy + distance * dirY;
  
  Serial.print("Sensor distance: ");
  Serial.println(distance);
  Serial.print("Estimated position: (");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.println(")");
  
  vectorToTello(-x, -y);
}

void stdMode(int x, int y) {
  int distance = sqrt(x * x + y * y); // calcola la distanza dal centro del pad
  Serial.print("Distance from center: ");
  Serial.println(distance);
  if (distance < acceptedDistanceError) { // se la distanza è inferiore alla soglia di accettazione, considera il drone centrato
    Serial.println("Drone Centered!");
    drone.land();
    landed = true;
  } else {
    Serial.println("Drone Not centered!");
    vectorToTello(-x, -y);
    landed = false; 
  }
}