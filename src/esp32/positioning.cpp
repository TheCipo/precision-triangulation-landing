#include "positioning.h"
#include "config.h"

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