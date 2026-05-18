#include "comunication.h"
#include <Wire.h>
#include <WiFi.h>
#include "config.h"
#include "drone.h"

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

void connectToWiFi(const char * ssid, const char * pwd) 
{
  Serial.println("Connecting to WiFi network: " + String(ssid));
  WiFi.disconnect(true);
  //register event handler

  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
  while (WiFi.status() != WL_CONNECTED) {
   delay(1000);
   Serial.print(".");
 }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  drone.init();
  connected = true;
}