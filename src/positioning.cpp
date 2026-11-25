#include "positioning.h"
#include "config.h"

void calculatePosition(int distances[3]) {
    if(distances[1] == ND || distances[2] == ND || distances[0] == ND) { //se i dati raccolti non sono validi, imposta la posizione del drone a ND
        Dx = ND;
        Dy = ND;
        return;
    }
    
    //calcolo della posizione del drone usando la formula della trilaterazione
    float A = 2 * (Bx - Ax);
    float B = 2 * (By - Ay);
    float C = sq(distances[0]) - sq(distances[1]) - sq(Ax) + sq(Bx) - sq(Ay) + sq(By);

    float D = 2 * (Cx - Bx);
    float E = 2 * (Cy - By);
    float F = sq(distances[1]) - sq(distances[2]) - sq(Bx) + sq(Cx) - sq(By) + sq(Cy);

    float denominator = A * E - B * D;

    if (denominator != 0) { //filtro per evitare divisioni per zero
      Dx = (C * E - B * F) / denominator;
      Dy = (A * F - C * D) / denominator;
    } else {
      Dx = ND;
      Dy = ND;
    }
}

void calculatePosition2Sensors(int idx1, float d1, int idx2, float d2) {
    float coords[3][2] = {{Ax, Ay}, {Bx, By}, {Cx, Cy}};
    float x1 = coords[idx1][0], y1 = coords[idx1][1];
    float x2 = coords[idx2][0], y2 = coords[idx2][1];

    float dx = x2 - x1;
    float dy = y2 - y1;
    float distCenters = sqrt(dx * dx + dy * dy);

    // Verifica che le due circonferenze si intersechino
    if (distCenters == 0 || distCenters > (d1 + d2) || distCenters < fabs(d1 - d2)) {
        Dx = ND;
        Dy = ND;
        return;
    }

    float a = (sq(d1) - sq(d2) + sq(distCenters)) / (2.0 * distCenters);
    float h = sqrt(max(0.0f, sq(d1) - sq(a)));

    float x0 = x1 + a * (dx / distCenters);
    float y0 = y1 + a * (dy / distCenters);

    // Due intersezioni possibili: seleziona quella piu' vicina all'origine (0, 0)
    float rx = -dy * (h / distCenters);
    float ry =  dx * (h / distCenters);

    float sol1_x = x0 + rx, sol1_y = y0 + ry;
    float sol2_x = x0 - rx, sol2_y = y0 - ry;

    if ((sq(sol1_x) + sq(sol1_y)) < (sq(sol2_x) + sq(sol2_y))) {
        Dx = sol1_x;
        Dy = sol1_y;
    } else {
        Dx = sol2_x;
        Dy = sol2_y;
    }
}
