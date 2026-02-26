#include <EEPROM.h>
#include <Wire.h>

#define MYADDRESS 10 // da cambiare con numero dell'arduino, per ordine non funzionalità
#define ND -1

#define DATATIMES 21 //numero di volte che vengono raccolti i dati di ogni sensore, meglio dispari

// EEPROM data
#define EPR_Ax 0
#define EPR_Ay 4
#define EPR_Bx 8
#define EPR_By 12
#define EPR_Cx 16
#define EPR_Cy 20
#define EPR_Max 24

// per setup
unsigned long timeout = 10000; // 10 secondi
unsigned long startTime;

// Definizione dei pin trig e echo per ciascun sensore
const int trigPins[3] = {2, 4, 6};
const int echoPins[3] = {3, 5, 7};

// Array delle distanze
int multiDistances[3][DATATIMES];
int distances[3] = {ND, ND, ND};
int MAXdistance = 80; // cm

// posizione dei sensori
float Ax = 0, Ay = 20.0;
float Bx = 17.32, By = -10;
float Cx = -17.32, Cy = -10;

String input = "";

// errore
bool error = true;
bool enter = false;

// coordinate punto
float x, y; // in elaborazione
float Dx, Dy; // da inviare

// Funzione per leggere la distanza di un singolo sensore
int readDistance(int trigPin, int echoPin) {
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  if (duration == 0) {
    error = true;
    return ND; // no echo
  }

  int dist = duration * 0.034 / 2;
  if (dist > MAXdistance) {
    error = true;
    return ND;
  }
  return dist;
}

// chiedi le coordinate dei sensori
void coordinateSensori() {
  float R = 0; //raggio dei 3 sensori
  Serial.print("R = ");
  input = readLine();
  input.trim();
  R = input.toFloat();

  Ax = 0;
  Ay = R;
  Bx = R * sqrt(3) / 2.0;
  By = -R / 2.0 ;
  Cx = -R * sqrt(3) / 2.0;
  Cy = -R / 2.0;

  Serial.print("MAXdistance = ");
  input = readLine();
  input.trim();
  MAXdistance = input.toFloat();
}

// funzione per mandare i dati a ESP32 DevKit
void I2C_DataSend() {
  Serial.print(MYADDRESS);
  Serial.print(" ha inviato X: ");
  Serial.print(Dx);
  Wire.write((byte *)&Dx, sizeof(Dx));
  Serial.print(" e Y: ");
  Serial.print(Dy);
  Wire.write((byte *)&Dy, sizeof(Dy));
  Serial.println(" al Master\n");
}

String readLine() {
  String datoricevuto = "";
  char c;

  while (Serial.available()) Serial.read(); // svuota buffer
  while (!Serial.available()); // aspetta input
  delay(10);
  while (Serial.available() > 0) {
    c = Serial.read();
    if (c == '\n') {
      break;
    } else {
      datoricevuto += c;
    }
  }
  return datoricevuto;
}

void setup() {
  Serial.begin(9600);
  Wire.begin(MYADDRESS);
  Wire.onRequest(I2C_DataSend);

  for (int i = 0; i < 3; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  EEPROM.get(EPR_Ax, Ax);
  EEPROM.get(EPR_Ay, Ay);
  EEPROM.get(EPR_Bx, Bx);
  EEPROM.get(EPR_By, By);
  EEPROM.get(EPR_Cx, Cx);
  EEPROM.get(EPR_Cy, Cy);
  EEPROM.get(EPR_Max, MAXdistance);

  Serial.println("Premi un tasto entro 10 secondi per entrare in modalità setup...");
  startTime = millis();
  while (Serial.available()) Serial.read();
  while (millis() - startTime < timeout && !enter) {
    if (Serial.available() > 0) {
      Serial.println("Entrato in modalità setup.");
      Serial.println("Coordinate dei Sensori:");
      Serial.print("Ax = "); Serial.print(Ax); Serial.print(" Ay = "); Serial.println(Ay);
      Serial.print("Bx = "); Serial.print(Bx); Serial.print(" By = "); Serial.println(By);
      Serial.print("Cx = "); Serial.print(Cx); Serial.print(" Cy = "); Serial.println(Cy);
      Serial.print("Distanza Massima = "); Serial.println(MAXdistance);
      Serial.println("Inserisci 0 per uscire, 1 per sceglierli provvisoriamente, 2 per modificare la EEPROM: ");

      while (Serial.available()) Serial.read();
      input = readLine();
      input.trim();

      switch (input.toInt()) {
        case 1:
          coordinateSensori();
          break;
        case 2:
          coordinateSensori();
          EEPROM.put(EPR_Ax, Ax);
          EEPROM.put(EPR_Ay, Ay);
          EEPROM.put(EPR_Bx, Bx);
          EEPROM.put(EPR_By, By);
          EEPROM.put(EPR_Cx, Cx);
          EEPROM.put(EPR_Cy, Cy);
          EEPROM.put(EPR_Max, MAXdistance);
          break;
        default:
          break;
      }
      enter = true;
    }
  }
  Serial.println("Avvio programma principale.");
}

void loop() {
  error = false;

  //read data from hc-sr04
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < DATATIMES; j++){
      multiDistances[i][j] = readDistance(trigPins[i], echoPins[i]);
      if(multiDistances[i][j] == ND){error = true;}
    }
  }

  if (!error) {
    //filtro errori (filtro a mediana)
    for(int k = 0; k < 3; k++){
      int mediana = 0;
      int indexMediana = DATATIMES/2;
      for (int i = 1; i < DATATIMES; ++i) {
        int key = multiDistances[k][i];
        int j = i - 1;
        while (j >= 0 && multiDistances[k][j] > key) {
            multiDistances[k][j + 1] = multiDistances[k][j];
            j = j - 1;
        }
        multiDistances[k][j + 1] = key;
      }
      mediana = multiDistances[k][indexMediana];
      distances[k] = mediana;
    }

    //calcolo della posizione
    float A = 2 * (Bx - Ax);
    float B = 2 * (By - Ay);
    float C = sq(distances[0]) - sq(distances[1]) - sq(Ax) + sq(Bx) - sq(Ay) + sq(By);

    float D = 2 * (Cx - Bx);
    float E = 2 * (Cy - By);
    float F = sq(distances[1]) - sq(distances[2]) - sq(Bx) + sq(Cx) - sq(By) + sq(Cy);

    float denominator = A * E - B * D;

    if (denominator != 0) {
      x = (C * E - B * F) / denominator;
      y = (A * F - C * D) / denominator;
      Dx = x;
      Dy = y;
    } else {
      x = ND;
      y = ND;
    }
  } else {
    x = ND;
    y = ND;
  }

  //invia i dati
  I2C_DataSend();

  delay(2000);
}
