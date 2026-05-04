# Precision Triangulation Landing 🛸

Sistema di localizzazione e guida di precisione per droni **DJI Tello** basato sulla trilaterazione mediante sensori a ultrasuoni e microcontrollore ESP32.

---

## 📌 Stato del Progetto
Il progetto si configura attualmente come un **Proof of Concept (PoC)** focalizzato sulla localizzazione spaziale. 

* **Completato:** Rilevamento distanze dai sensori HC-SR04, filtraggio dati mediante mediana su 21 campioni per ridurre il rumore, calcolo delle coordinate $(x, y)$ tramite trilaterazione, sistema di configurazione runtime per la calibrazione dei sensori e storage persistente delle impostazioni su flash ESP32. I risultati vengono visualizzati su monitor seriale.
* **In Sviluppo:** Implementazione effettiva del sistema di controllo remoto via WiFi per il drone DJI Tello, inclusi i comandi di volo e il controllo di centraggio per l'atterraggio di precisione.

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

## 🔧 Logica del Codice: Dai Sensori al Posizionamento

Il codice segue un flusso logico strutturato per garantire misurazioni accurate e calcolo della posizione. Ecco i passaggi principali:

### 1. **Setup e Configurazione (Fase di Avvio)**
   - Caricamento delle coordinate dei sensori dalla memoria flash (ESP32 Preferences).
   - Possibilità di accedere a un menu di configurazione seriale entro 10 secondi dall'avvio per calibrare i sensori (posizione in triangolo equilatero con raggio configurabile).
   - Inizializzazione dei pin GPIO per i 3 sensori HC-SR04 (trigger: 23,19,17; echo: 22,18,16).

### 2. **Acquisizione Dati dai Sensori**
   - Per ciascun sensore, invio di un impulso di trigger di 10µs.
   - Misura della durata dell'eco con timeout di 15ms.
   - Conversione del tempo in distanza: `distanza = durata / 59` (in cm).
   - Validazione: scarto valori fuori dal range [MINdistance, MAXdistance] (default 2-80 cm), restituzione di `ND` (-9999) per letture invalide.

### 3. **Filtraggio dei Dati**
   - Raccolta di 21 campioni per sensore con delay di 60ms tra ciascuno.
   - Applicazione del filtro mediano: ordinamento dei valori validi e selezione del valore centrale.
   - Scarto automatico degli outlier estremi, garantendo stabilità.

### 4. **Calcolo della Posizione (Trilaterazione)**
   - Risoluzione del sistema di equazioni delle intersezioni di cerchi:
     ```
     (x - Ax)² + (y - Ay)² = d₀²
     (x - Bx)² + (y - By)² = d₁²
     (x - Cx)² + (y - Cy)² = d₂²
     ```
   - Uso di algebra lineare per calcolare le coordinate (Dx, Dy).
   - Validazione: controllo della matrice dei coefficienti per evitare divisioni per zero.
   - Output: posizione del drone o (ND, ND) se insolubile.

### 5. **Output e Monitoraggio**
   - Stampa seriale delle distanze filtrate e delle coordinate calcolate.
   - Pronto per il ciclo successivo (loop continuo).

### 6. **Controllo Drone (Placeholder)**
   - Strutture preparate per comunicazione WiFi con DJI Tello e comandi di volo.
   - Non ancora implementato: invio comandi di movimento e centraggio basato sulla posizione calcolata.

---

## 📂 Struttura delle Cartelle
* `src/`: Contiene il codice sorgente.
* `docs/`: Documentazione di progetto e presentazioni scolastiche.
* `libs/`: Eventuali librerie per la gestione del sensore o del drone.

---

## ⚠️ Limitazioni Attuali
* **Visibilità:** Se il drone non è nel raggio d'azione di tutti e tre i sensori contemporaneamente, il sistema non è in grado di stimare la posizione (richiede 3 distanze valide).
* **Comunicazione:** Il controllo remoto del drone via WiFi non è ancora implementato; attualmente sono presenti solo placeholder per le funzioni di comunicazione e centraggio. La calibrazione avviene esclusivamente via interfaccia seriale.
