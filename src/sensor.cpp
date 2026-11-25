#include "sensor.h"
#include "config.h"

int readDistance(int trigPin, int echoPin) {
  long duration; //variabile per memorizzare la durata del segnale di ritorno
  //manda un impulso di 10 microsecondi al pin di trigger
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(4);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, MAXtimeout); //attendi il segnale di ritorno e memorizza la durata, con un timeout per evitare blocchi
  if (duration == 0) return ND;

  long int rawDist = duration / 58; // Distanza obliqua reale in cm
  if (rawDist > MAXdistance) return ND;
  if (rawDist < MINdistance) return ND;

  // Proiezione sul piano orizzontale XY in base all'angolo di inclinazione (0° = orizzontale)
  float rad = sensorAngle * PI / 180.0f;
  int horizontalDist = (int)round(rawDist * cos(rad));

  return horizontalDist;
}
