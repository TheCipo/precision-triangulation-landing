#include "setup.h"
#include "config.h"
#include "comunication.h"
#include <Preferences.h>
#include <Wire.h>

void coordinateSensori() { //leggere le cordinate dei sensori da seriale
  Serial.print("R (raggio) = "); //richiesta del raggio del triangolo formato dai sensori, che determina la posizione dei sensori stessi
  String input = readLine();
  float R = input.toFloat();

  //calcolo delle coordinate dei sensori in base al raggio e alla disposizione circolare
  Ax = 0; Ay = R;
  Bx = R * sqrt(3) / 2.0; By = -R / 2.0;
  Cx = -R * sqrt(3) / 2.0; Cy = -R / 2.0;

  //richiesta della distanza massima e minima misurabile dai sensori
  Serial.print("MAXdistance = ");
  input = readLine();
  MAXdistance = input.toInt();
  Serial.print("MINdistance = ");
  input = readLine();
  MINdistance = input.toInt();
  Serial.print("padZero X = ");
  input = readLine();
  padVector[0] = input.toInt();
  Serial.print("padZero Y = ");
  input = readLine();
  padVector[1] = input.toInt();
  Serial.print("pad index = ");
  input = readLine();
  padIndex = input.toInt();
}

void menuConfigurazione() { 
  //stampa del menu di configurazione
  Serial.println("Entrato in modalità setup.");
  Serial.println("Coordinate dei Sensori:");
  Serial.print("Ax = "); Serial.print(Ax); Serial.print(" Ay = "); Serial.println(Ay);
  Serial.print("Bx = "); Serial.print(Bx); Serial.print(" By = "); Serial.println(By);
  Serial.print("Cx = "); Serial.print(Cx); Serial.print(" Cy = "); Serial.println(Cy);
  Serial.print("Distanza Massima = "); Serial.println(MAXdistance);
  Serial.print("Distanza Minima = "); Serial.println(MINdistance);
  Serial.print("PadZero X = "); Serial.println(padVector[0]);
  Serial.print("PadZero Y = "); Serial.println(padVector[1]);
  Serial.print("pad index = "); Serial.println(padIndex);
  Serial.println("Inserisci 0 per uscire, 1 per sceglierli provvisoriamente, 2 per modificare la EEPROM/FLASH: ");
  
  // selettore della modalità di configurazione
  String input = readLine();
  int scelta = input.toInt();

  if (scelta == 1 || scelta == 2) {
    coordinateSensori(); //impostazione delle coordinate dei sensori e delle variabili di riferimento
    if (scelta == 2) { //salvataggio delle coordinate dei sensori e delle variabili di riferimento nella EEPROM/FLASH
      storage.putFloat("Ax", Ax);
      storage.putFloat("Ay", Ay);
      storage.putFloat("Bx", Bx);
      storage.putFloat("By", By);
      storage.putFloat("Cx", Cx);
      storage.putFloat("Cy", Cy);
      storage.putInt("MAXdistance", MAXdistance);
      storage.putInt("MINdistance", MINdistance);
      storage.putInt("padZeroX", padVector[0]);
      storage.putInt("padZeroY", padVector[1]);
      storage.putInt("padIndex", padIndex);
      Serial.println("Salvato!");
    }
  }
}

void loadSetup() {
  //caricamento delle coordinate dei sensori e delle variabili di riferimento dalla EEPROM/FLASH
  Ax = storage.getFloat("Ax", 0);
  Ay = storage.getFloat("Ay", 0);
  Bx = storage.getFloat("Bx", 0);
  By = storage.getFloat("By", 0);
  Cx = storage.getFloat("Cx", 0);
  Cy = storage.getFloat("Cy", 0);
  MAXdistance = storage.getInt("MAXdistance", 80);
  MINdistance = storage.getInt("MINdistance", 0);
  padVector[0] = storage.getInt("padZeroX", 0);
  padVector[1] = storage.getInt("padZeroY", 0);
  padIndex = storage.getInt("padIndex", 0);
}