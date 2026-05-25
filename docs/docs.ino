// HC-SR04 multi-test. Supporta 1..3 sensori.
const int NUM_SENSORS = 3;
const int trigPins[NUM_SENSORS] = {23, 19, 17};
const int echoPins[NUM_SENSORS] = {22, 18, 16};

const int SAMPLES = 5;        // letture da mediannare/averagizzare
const unsigned long TIMEOUT = 30000UL; // microsecondi timeout per pulseIn

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_SENSORS; ++i) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    digitalWrite(trigPins[i], LOW);
  }
  delay(100);
  Serial.println("HC-SR04 sensors test started");
}

float readDistanceCm(int idx) {
  // invia impulso trigger
  digitalWrite(trigPins[idx], LOW);
  delayMicroseconds(2);
  digitalWrite(trigPins[idx], HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPins[idx], LOW);

  unsigned long duration = pulseIn(echoPins[idx], HIGH, TIMEOUT); // microsecondi
  if (duration == 0) return -1.0f; // timeout / no echo
  float distanceCm = duration / 58.0f; // formula comune per HC-SR04
  return distanceCm;
}

void loop() {
  for (int i = 0; i < NUM_SENSORS; ++i) {
    float sum = 0.0f;
    int valid = 0;
    for (int s = 0; s < SAMPLES; ++s) {
      float d = readDistanceCm(i);
      if (d >= 0.0f) {
        sum += d;
        valid++;
      }
      delay(20); // piccola pausa fra le letture
    }
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    if (valid == 0) {
      Serial.println("no echo / out of range");
    } else {
      float avg = sum / valid;
      Serial.print(avg, 1);
      Serial.println(" cm");
    }
    delay(100);
  }
  Serial.println("---");
  delay(800);
}