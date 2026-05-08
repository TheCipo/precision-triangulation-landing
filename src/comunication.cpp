#include "comunication.h"
#include <Wire.h>
#include <WiFi.h>
#include "config.h"

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
  WiFi.onEvent(WiFiEvent);

  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
  switch (event){
  case SYSTEM_EVENT_STA_GOT_IP: //When connected set
    Serial.print("WiFi connected! IP address: ");
    Serial.println(WiFi.localIP()); //initialise Tello after we are connected
    
    drone.init();
    
    connected = true;
    break;
    
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    connected = false;
    break;
  }
}
