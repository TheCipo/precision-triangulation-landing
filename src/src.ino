#include <Wire.h>
#include <WiFi.h>
#include "config.h"
#include "positioning.h"
#include "drone.h"
#include "comunication.h"
#include "setup.h"
#include "centering_control.h"
#include "sensor.h"
#include "filter.h"
#include "secrets.h"

// Inizializzazione variabili globali
Preferences storage;
float Ax, Ay, Bx, By, Cx, Cy;
float Dx, Dy;
int MAXdistance = 140;
int MINdistance = 2;
float sensorAngle = 20.0f; // Inclinazione default 20° (modificabile dal menu setup)
int multiDistances[3][DATATIMES];
int finalDistances[3];
bool error = false;
int errorCount = 0;
int padIndex = 0;
int padVector[2];
bool connected = false;
bool landed = false;
int acceptedDistanceError = 5;

// Controllo non bloccante del Monitor Seriale per atterraggio manuale di emergenza
bool checkSerialLand() {
  static String serialBuffer = "";
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      serialBuffer.trim();
      serialBuffer.toLowerCase();
      if (serialBuffer == "land" || serialBuffer == "l" || serialBuffer == "stop") {
        Serial.println("\n[COMANDO MANUALE] Ricevuto 'land' da Serial Monitor! Atterraggio immediato...");
        serialBuffer = "";
        drone.land();
        landed = true;
        return true;
      }
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }
  return false;
}

// Filtro di plausibilita' fisica parametrico al raggio R (Ay) con reset temporale post-recupero:
// scarta echi di rimbalzo fuori area o "teletrasporti"
bool isPositionPlausible(float x, float y) {
  static bool hasLastValid = false;
  static float lastValidX = 0.0f;
  static float lastValidY = 0.0f;
  static unsigned long lastValidTime = 0;

  // 1. Controllo raggio massimo dinamico (Ay = R + 5 cm di tolleranza)
  float maxAllowedRadius = Ay + 5.0f;
  float distFromCenter = sqrt(x * x + y * y);
  if (distFromCenter > maxAllowedRadius) {
    Serial.print("[SCARTATA] Posizione fuori area sensori (");
    Serial.print(x); Serial.print(", "); Serial.print(y);
    Serial.print(") > Raggio max "); Serial.print(maxAllowedRadius);
    Serial.println(" cm - probabile eco di rimbalzo, rileggo...");
    return false;
  }

  // 2. Controllo anti-teletrasporto: salto istantaneo > 35 cm SOLO se l'ultima lettura
  // risale a meno di 4 secondi fa (evita falsi scarti dopo un AUTO-RECUPERO)
  if (hasLastValid && (millis() - lastValidTime < 4000)) {
    float jumpDist = sqrt(sq(x - lastValidX) + sq(y - lastValidY));
    if (jumpDist > 35.0f) {
      Serial.print("[SCARTATA] Salto anomalo di ");
      Serial.print(jumpDist);
      Serial.println(" cm rispetto all'ultima lettura, rileggo...");
      return false;
    }
  }

  lastValidX = x;
  lastValidY = y;
  lastValidTime = millis();
  hasLastValid = true;
  return true;
}

void setup() {
  storage.begin("landing", false);
  Wire.begin();
  Serial.begin(9600);
  
  Serial.println("Invia un tasto entro 10s per il setup...");
  unsigned long startT = millis();
  loadSetup();
  while (millis() - startT < 10000) {
    if (Serial.available()) {
      menuConfigurazione(); //apertura del menu di configurazione se viene ricevuto un input entro 10 secondi
      break;
    }
  }
  storage.end(); //chiusura del Preferences storage

  for(int i = 0; i < 3; i++){ //configurazione dei pin dei sensori
    pinMode(trigPins[i], OUTPUT);
    digitalWrite(trigPins[i], LOW);
    pinMode(echoPins[i], INPUT);
  }

  connectToWiFi(networkName, networkPswd);
  resetTello();
  Serial.println("[SICUREZZA] Scrivi 'land' (o 'l') + Invio nel Monitor Seriale in qualsiasi momento per atterrare.");
}

void loop() {
  // 1. Se il drone e' gia' atterrato (al centro o tramite 'land' manuale), blocca tutto subito
  if (checkSerialLand() || landed) {
    return;
  }

  // 2. Auto-recupero in volo: se il drone esce dall'area per > MAXERRORS cicli
  if (errorCount > MAXERRORS) {
    Serial.println("[AUTO-RECUPERO] Drone fuori area: rientro sul Mission Pad e riavvio ciclo...");
    resetTello();
    errorCount = 0;
    return;
  }

  // 3. Reset dell'array globale
  for (int i = 0; i < 3; i++) {
    finalDistances[i] = ND;
  }

  // 4. Lettura alternata A -> B -> C per evitare cross-talk ed echi residui
  for (int j = 0; j < DATATIMES; j++) {
    for (int i = 0; i < 3; i++) {
      if (checkSerialLand()) return;
      
      int d = readDistance(trigPins[i], echoPins[i]);
      multiDistances[i][j] = d;
      delay(60);
    }
  }

  // 5. Filtro mediano/media sui campioni raccolti
  for (int i = 0; i < 3; i++) {
    finalDistances[i] = filter(multiDistances[i]);
  }

  // Stampa diagnostica delle distanze gia' proiettate sul piano orizzontale
  Serial.print("Distanze orizzontali [A, B, C] (angolo ");
  Serial.print(sensorAngle);
  Serial.print("°): ");
  for (int i = 0; i < 3; i++) {
    Serial.print(finalDistances[i]);
    if (i < 2) Serial.print(", ");
  }
  Serial.println();

  // 6. Conta quanti sensori hanno restituito una misura valida
  int validCount = 0;
  int validIndices[3];
  for (int i = 0; i < 3; i++) {
    if (finalDistances[i] != ND) {
      validIndices[validCount++] = i;
    }
  }

  static int lastSingleSensor = -1;
  static int lastSingleDist = -9999;
  static int stableSingleCount = 0;

  if (validCount == 3) {
    stableSingleCount = 0;
    calculatePosition(finalDistances);
    if (isPositionPlausible(Dx, Dy)) {
      errorCount = 0;
      Serial.print("Posizione (3 sensori - Trilaterazione): (");
      Serial.print(Dx); Serial.print(", "); Serial.print(Dy); Serial.println(")");
      stdMode(Dx, Dy);
    } else {
      delay(200);
    }

  } else if (validCount == 2) {
    stableSingleCount = 0;
    calculatePosition2Sensors(validIndices[0], finalDistances[validIndices[0]],
                              validIndices[1], finalDistances[validIndices[1]]);
    if (Dx != ND && Dy != ND && isPositionPlausible(Dx, Dy)) {
      errorCount = 0;
      Serial.print("Posizione (2 sensori - Bilaterazione): (");
      Serial.print(Dx); Serial.print(", "); Serial.print(Dy); Serial.println(")");
      stdMode(Dx, Dy);
    } else {
      delay(200);
    }

  } else if (validCount == 1) {
    int sIdx = validIndices[0];
    int d = finalDistances[sIdx];
    float R = Ay; // Raggio attuale del triangolo (es. 51 o 40 cm)

    // Verifica coerenza: stesso sensore e variazione <= 8 cm rispetto al ciclo precedente
    if (sIdx == lastSingleSensor && abs(d - lastSingleDist) <= 8 && d >= 10 && d <= (2 * R + 15)) {
      stableSingleCount++;
    } else {
      stableSingleCount = 1;
      lastSingleSensor = sIdx;
    }
    lastSingleDist = d;

    if (stableSingleCount >= 2) {
      // Proiezione radiale lungo il cono da 15° del sensore attivo verso il centro (0,0)
      float sensorCoords[3][2] = {{Ax, Ay}, {Bx, By}, {Cx, Cy}};
      float ratio = 1.0f - ((float)d / R);
      Dx = sensorCoords[sIdx][0] * ratio;
      Dy = sensorCoords[sIdx][1] * ratio;

      // Vettore errore verso (0,0) con passo cautelativo limitato a max 15 cm
      int errX = constrain((int)round(-Dx), -35, 35);
      int errY = constrain((int)round(-Dy), -35, 35);

      const char* names[3] = {"A (Fronte)", "B (Destra)", "C (Sinistra)"};
      Serial.println("==================================================");
      Serial.print("[AVVICINAMENTO RADIALE 1 SENSORE] Sensore ");
      Serial.print(names[sIdx]);
      Serial.print(" | d = "); Serial.print(d); Serial.println(" cm");
      Serial.print("  -> Posizione stimata nel cono: (");
      Serial.print(Dx); Serial.print(", "); Serial.print(Dy); Serial.println(") cm");
      Serial.println("==================================================");

      if (abs(errX) > acceptedDistanceError || abs(errY) > acceptedDistanceError) {
        vectorToTello(errX, errY);
      }
      errorCount = 0;
      stableSingleCount = 0; // Attende una nuova conferma dopo il movimento
    } else {
      errorCount++;
      Serial.print("[ATTESA CONO 1 SENSORE] Conferma lettura (");
      Serial.print(stableSingleCount);
      Serial.println("/2)...");
      delay(200);
    }

  } else {
    stableSingleCount = 0;
    errorCount++;
    Serial.print("Impossibile calcolare la posizione (0 sensori validi), errore: ");
    Serial.println(errorCount);
  }
}
