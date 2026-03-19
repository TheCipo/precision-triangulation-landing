#include "sensor.h"
#include "config.h"

int readDistance(int trigPin, int echoPin) {
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, MAXtimeout); 
  if (duration == 0) return ND;

  long int dist = duration / 59;
  if (dist > MAXdistance) return ND;
  if (dist < MINdistance) return ND; // Considera distanze inferiori a 5 cm come ND
  return dist;
}
