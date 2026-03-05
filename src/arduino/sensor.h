#ifndef SENSOR_H
#define SENSOR_H

int readDistance(int trigPin, int echoPin);
void calculatePosition(int distances[3]);

#endif