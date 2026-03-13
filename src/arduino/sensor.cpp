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
