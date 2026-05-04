#include "comunication.h"
#include <Wire.h>

String readLine(){
  String datoricevuto = ""; //crea la stringa vuota
  while (Serial.available()) Serial.read();  //pulisci la seriale da eventuali dati residui
  while (!Serial.available()); //attendi che ci siano dati disponibili
  delay(10); 
  while (Serial.available() > 0) { //leggi i dati finché ci sono
    char c = Serial.read();
    if (c == '\n') break;
    datoricevuto += c; //aggiungi il carattere alla stringa
  }
  return datoricevuto; //ritorna la stringa letta
}