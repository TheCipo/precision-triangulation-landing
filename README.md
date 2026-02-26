# Precision Triangulation Landing 🛸

Sistema di localizzazione e guida di precisione per droni **DJI Tello** basato sulla trilaterazione mediante sensori a ultrasuoni e microcontrollori ESP32/Arduino.

---

## 📌 Stato del Progetto
Il progetto si configura attualmente come un **Proof of Concept (PoC)** focalizzato sulla localizzazione spaziale. 

* **Completato:** Rilevamento distanze, filtraggio dati (mediana) e calcolo coordinate $(x, y)$ visualizzate su monitor seriale.
* **In Sviluppo:** Trasmissione dati tra Arduino e ESP32 e implementazione del sistema di controllo remoto via WiFi per il drone.

---

## 📐 Il Concetto Teorico: La Trilaterazione
A differenza della "triangolazione" (che usa gli angoli), questo progetto utilizza la **trilaterazione**, basata esclusivamente sulla misura delle distanze.

### Il Principio Geometrico
Il sistema si basa su tre punti fissi (i sensori HC-SR04) situati a coordinate note sulla base di atterraggio. 
1. Ogni sensore misura la distanza dal drone. Immaginiamo questa distanza come il **raggio di una sfera** che ha come centro il sensore stesso.
2. Il drone deve trovarsi in un punto preciso della superficie di questa sfera.
3. Utilizzando due sensori, l'intersezione delle due sfere forma un cerchio di possibili posizioni.
4. Utilizzando un **terzo sensore**, l'intersezione tra il cerchio precedente e la terza sfera identifica un punto univoco nello spazio (o due, di cui uno viene scartato perché matematicamente impossibile, ad esempio se risulta sotto il livello del suolo).



### Ottimizzazione dei Dati
Le letture degli ultrasuoni possono essere "sporche" a causa del rumore acustico dei motori del drone. Per risolvere questo problema, il sistema non utilizza la media aritmetica, ma il **calcolo della mediana** su un set di letture multiple. Questo permette di scartare automaticamente i valori totalmente errati (outlier) garantendo una coordinata $(x, y)$ stabile.

---

## 🛠️ Architettura Hardware
Il sistema è diviso in due unità principali per ottimizzare la gestione dei compiti:

* **Unità di Rilevamento (Arduino Uno):** Gestisce il timing critico dei 3 sensori HC-SR04 e pulisce il segnale.
* **Unità di Calcolo e Comando (ESP32):** Riceve le distanze, risolve le equazioni geometriche per trovare la posizione e gestisce la connessione WiFi UDP con il DJI Tello.

---

## 📂 Struttura delle Cartelle
* `src/`: Contiene il codice sorgente.
    * `arduino.ino`: Codice per la lettura sensori e calcolo mediana.
    * `esp32.ino`: Logica di calcolo posizione e interfaccia drone (WIP).
* `docs/`: Documentazione di progetto e presentazioni scolastiche.
* `libs/`: Eventuali librerie per la gestione del sensore o del drone.

---

## ⚠️ Limitazioni Attuali
* **Visibilità:** Se il drone non è nel raggio d'azione di tutti e tre i sensori contemporaneamente, il sistema non è in grado di stimare la posizione (richiede 3 distanze valide).
* **Comunicazione:** La parte di invio comandi di volo al drone non è ancora stata testata in ambiente reale.