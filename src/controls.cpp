#include "controls.h"
#include "config.h"
#include "comunication.h"
#include "Tello.h"

void emergencyStop() {
  drone.land();
  Serial.println("Emergency stop activated!");
	while (true) {
		delay(1000); // Attendi un secondo prima di ripetere il messaggio
		Serial.println("Emergency stop activated!, press reset to restart"); // Stampa il messaggio di emergenza ogni secondo
	}
}

