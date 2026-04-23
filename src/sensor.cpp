#include "sensor.h"
#include "config.h"

int readDistance(int trigPin, int echoPin) {
  long duration; //variabile per memorizzare la durata del segnale di ritorno
  //manda un impulso di 10 microsecondi al pin di trigger
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, MAXtimeout); //attendi il segnale di ritorno e memorizza la durata, con un timeout per evitare blocchi
  if (duration == 0) return ND;

  long int dist = duration / 59; //calcola la distanza in cm (velocità del suono è circa 340 m/s, quindi 29 microsecondi per cm andata e ritorno)
  if (dist > MAXdistance) return ND; // Considera distanze superiori alla distanza massima come ND 
  if (dist < MINdistance) return ND; // Considera distanze inferiori alla distanza minima come ND
  return dist;
}
