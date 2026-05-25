#ifndef DRONE_H
#define DRONE_H

#include "config.h"
#include "tello.h"

extern Tello drone;

void resetTello();
void vectorToTello(int x, int y);

#endif