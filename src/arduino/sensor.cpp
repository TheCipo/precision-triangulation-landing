#include "sensor.h"
#include "config.h"

int readDistance(int trigPin, int echoPin) {
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000); 
  if (duration == 0) return ND;

  int dist = duration * 0.034 / 2;
  return (dist > MAXdistance) ? ND : dist;
}

void calculatePosition(int distances[3]) {
    float A = 2 * (Bx - Ax);
    float B = 2 * (By - Ay);
    float C = sq(distances[0]) - sq(distances[1]) - sq(Ax) + sq(Bx) - sq(Ay) + sq(By);

    float D = 2 * (Cx - Bx);
    float E = 2 * (Cy - By);
    float F = sq(distances[1]) - sq(distances[2]) - sq(Bx) + sq(Cx) - sq(By) + sq(Cy);

    float denominator = A * E - B * D;

    if (denominator != 0) {
      Dx = (C * E - B * F) / denominator;
      Dy = (A * F - C * D) / denominator;
    } else {
      Dx = ND;
      Dy = ND;
    }
}