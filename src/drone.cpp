#include "drone.h"
#include "Tello.h"
#include "config.h"

Tello drone;

// Fattore di compensazione asimmetrica: il Tello percorre meno strada in retromarcia (-X) rispetto all'avanti (+X).
// 1.35f significa +35% di comando quando telloX < 0 (tarabile tra 1.25 e 1.45 in base ai test).
const float BACKWARD_COMPENSATION = 1.35f;

// Manovra di ripristino IMU: sale di 30 cm per riagganciare il sensore ottico inferiore,
// fa un micro-assestamento orizzontale e ridiscende di 30 cm solo se la salita e' riuscita.
void recoverImu() {
  Serial.println("========================================");
  Serial.println("[RECUPERO IMU] Rilevato 'No valid imu'! Avvio procedura:");
  Serial.println("  1. Salgo di 30 cm per riagganciare il sensore ottico...");
  bool climbed = drone.up(30);
  delay(500);

  Serial.println("  2. Micro-spostamento di calibrazione (forward 20 -> back 20)...");
  drone.forward(20);
  delay(300);
  drone.back(20);
  delay(400);

  if (climbed) {
    Serial.println("  3. Ritorno alla quota di misura (down 30)...");
    drone.down(30);
    delay(1200);
  } else {
    Serial.println("  3. Salita non eseguita: mantengo quota attuale!");
  }
  Serial.println("[RECUPERO IMU] Procedura completata!");
  Serial.println("========================================");
}

// Esegue drone.go() gestendo 'No valid imu' e arrestando il ciclo se la batteria e' scarica
bool safeGo(int x, int y, int z, int speed) {
  if (drone.go(x, y, z, speed)) {
    return true;
  }

  if (drone.lastResponse.find("Auto land") != std::string::npos ||
      drone.lastResponse.find("Motor stop") != std::string::npos) {
    Serial.println("[SICUREZZA BATTERIA] Rilevato Auto land / Motor stop: arresto ciclo.");
    landed = true;
    return false;
  }

  if (drone.lastResponse.find("No valid imu") != std::string::npos) {
    recoverImu();
    Serial.println("[RECUPERO IMU] Riprovo il comando di movimento...");
    return drone.go(x, y, z, speed);
  }

  return false;
}

void vectorToTello(int x, int y) {
  if (x < -9000 || y < -9000 || landed) {
    return;
  }

  // Guadagno proporzionale Kp = 0.75 per evitare sovracorrezioni attorno al centro (0,0)
  const float Kp = 0.75f;
  const int speed = 20; // Velocita' bassa (20 cm/s) per movimenti dolci

  int corrX = (int)round(x * Kp);
  int corrY = (int)round(y * Kp);

  // Tetto massimo di sicurezza: impedisce qualsiasi singolo balzo superiore a 25 cm
  corrX = constrain(corrX, -25, 25);
  corrY = constrain(corrY, -25, 25);

  // Garantisce almeno 2 cm di spostamento se l'errore reale e' diverso da zero
  if (x != 0 && corrX == 0) corrX = (x > 0) ? 2 : -2;
  if (y != 0 && corrY == 0) corrY = (y > 0) ? 2 : -2;

  Serial.println("========================================");
  Serial.println("RIFERIMENTO PRE-MOVIMENTO:");
  Serial.print("  -> Posizione rilevata drone (Dx, Dy): (");
  Serial.print(Dx);
  Serial.print(", ");
  Serial.print(Dy);
  Serial.println(") cm");
  Serial.print("  -> Vettore errore target (x, y):     (");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.println(") cm");
  Serial.print("  -> Correzione smorzata Kp=0.75:       (");
  Serial.print(corrX);
  Serial.print(", ");
  Serial.print(corrY);
  Serial.println(") cm");

  // Nel SDK Tello EDU: go(forward/back, left/right, up/down, speed)
  // Sensore A (0, +50) frontale -> forward = +telloX = +corrY
  // Sensore B (+43.3, -25) a destra -> right = -telloY -> telloY = -corrX
  int telloX = corrY;
  int telloY = -corrX;

  if (abs(telloX) < 20 && abs(telloY) < 20) {
    if (telloX >= 0) {
      // Se il drone e' gia' molto indietro (Dy < -10 cm) e deve avanzare almeno di 8 cm,
      // NON lo mandiamo ancora piu' indietro: fa un singolo passo diretto di 20 cm in avanti!
      if (Dy < -10.0f && telloX >= 8) {
        Serial.print("  -> Avvicinamento diretto da dietro (evita arretramento): go(20, ");
        Serial.print(telloY); Serial.println(", 0, 20)");
        Serial.println("========================================");
        safeGo(20, telloY, 0, speed);
      } else {
        // Vicino al centro (Dy >= -10 cm): usa la micro-manovra compensata a 2 passi
        int backStep = (int)round(-20 * BACKWARD_COMPENSATION);
        int step2X = 20 + telloX;

        Serial.println("  -> Micro-manovra compensata (arretra poi centra):");
        Serial.print("  -> Comando 1/2 (arretramento compensato): go(");
        Serial.print(backStep); Serial.println(", 0, 0, 20)");
        Serial.println("========================================");

        if (safeGo(backStep, 0, 0, speed)) {
          delay(250);
          Serial.print("  -> Comando 2/2 (centraggio finale):       go(");
          Serial.print(step2X); Serial.print(", "); Serial.print(telloY); Serial.println(", 0, 20)");
          safeGo(step2X, telloY, 0, speed);
        } else {
          Serial.println("  -> ERRORE Comando 1/2 fallito: centraggio annullato!");
        }
      }
    } else {
      // Deve ARRETRARE di telloX (-19..-1 cm): compensa l'intero spostamento negativo
      int step1X = (int)round((-20 + telloX) * BACKWARD_COMPENSATION);
      Serial.println("  -> Micro-manovra compensata (arretra e rientra):");
      Serial.print("  -> Comando 1/2 (arretramento compensato): go(");
      Serial.print(step1X); Serial.print(", "); Serial.print(telloY); Serial.println(", 0, 20)");
      Serial.println("========================================");

      if (safeGo(step1X, telloY, 0, speed)) {
        delay(250);
        Serial.println("  -> Comando 2/2 (rientro):                 go(20, 0, 0, 20)");
        safeGo(20, 0, 0, speed);
      } else {
        Serial.println("  -> ERRORE Comando 1/2 fallito: rientro annullato!");
      }
    }
  } else {
    // Spostamento diretto >= 20 cm: compensa solo se deve andare all'indietro (telloX < 0)
    int finalTelloX = (telloX < 0) ? (int)round(telloX * BACKWARD_COMPENSATION) : telloX;

    Serial.print("  -> Comando unico diagonale");
    if (telloX < 0) Serial.print(" (retromarcia compensata)");
    Serial.print(": go(");
    Serial.print(finalTelloX); Serial.print(", "); Serial.print(telloY); Serial.println(", 0, 20)");
    Serial.println("========================================");

    safeGo(finalTelloX, telloY, 0, speed);
  }

  // Pausa di stabilizzazione hovering prima della prossima lettura ad ultrasuoni
  delay(800);
}

void resetTello() {
  if (!connected) {
    Serial.println("Drone non connesso, impossibile decollare.");
    return;
  }
  drone.mon(); // abilita riconoscimento pad
  delay(300);
  drone.mdirection(2); // abilita sensori sia frontale che verticale
  delay(300);
  drone.takeoff(); // decolla
  delay(500);
  drone.flyto(0, 0, 50, 30, padIndex); // riconosce il pad e si porta a quota 50 cm
  delay(800);
  drone.forward(30);
  delay(800);
  drone.forward(30);
  delay(800);
  drone.down(20);
  delay(1500);
}

