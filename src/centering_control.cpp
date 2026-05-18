#include "centering_control.h"
#include "drone.h"
#include "tello.h"
#include "config.h"

void degradedMode(int distance, int index) {
  Serial.println("Degraded mode!");
  int deg = 120 * index; // calcola l'angolo in base all'indice del sensore
  //calcola il vettore di movimento in base alla distanza e all'angolo
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("Angle: ");
  Serial.println(deg);
  float cosDeg = cos(deg * PI / 180);
  float sinDeg = sin(deg * PI / 180);
  int x = distance * cosDeg;
  int y = distance * sinDeg;
  Serial.print("Vector: (");
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