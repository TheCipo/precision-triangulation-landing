# 🛸 Guida alla Collaborazione: Drone Tello & ESP32 Localizer

Benvenuti nel team! Questa guida spiega come useremo GitHub tramite **VS Code** e l'**Interfaccia Web** per costruire il sistema di atterraggio autonomo senza fare casini.

---

## 🏗️ Tour Rapido delle Schede (Web)

Quando aprite il progetto su GitHub dal browser, ecco cosa dovete guardare:

* **📋 Projects**: La nostra "Torre di Controllo". Qui ci sono le card da spostare tra *Todo*, *In Progress* e *Done*. Guardatela ogni giorno.
* **🟢 Issues**: Qui descriviamo i compiti. Se trovi un problema o hai un'idea, apri una Issue.
* **🔀 Pull Requests**: Qui si revisiona il codice. Quando finisci un compito su VS Code, la richiesta di "unione" apparirà qui.
* **📖 Wiki**: Qui scriveremo la teoria (formule di trilaterazione, pinout dell'ESP32, protocolli UDP).

---

## 💻 Come lavorare con VS Code (Senza terminale)

Usate l'icona **"Source Control"** (quella con i tre nodi) sul lato sinistro di VS Code.

### 1. Prima di iniziare
In basso a sinistra, clicca sull'icona delle frecce circolari o sulla nuvoletta per fare il **Sync/Pull**. Questo scarica le modifiche fatte dai tuoi compagni mentre dormivi.

### 2. Creare un Branch (Il tuo spazio sicuro)
Non scrivere mai sul ramo `main`. 
* Clicca in basso a sinistra dove c'è scritto `main`.
* Seleziona **"+ Create new branch..."**.
* Chiamalo col tuo compito (es: `feature-wifi-tello` o `fix-mediana-noise`).

### 3. Salvare il lavoro (Commit & Push)
Mentre scrivi il codice:
1.  Vai nella scheda "Source Control".
2.  Clicca sul **+** accanto ai file modificati (Stage Changes).
3.  Scrivi un messaggio chiaro nel box (es: "Aggiunto calcolo coordinate X,Y").
4.  Clicca su **Commit**.
5.  Clicca su **Publish Branch** (o la nuvoletta) per inviarlo su GitHub.

---

## 🔄 Il Flusso delle Pull Request (Revisione)

Una volta fatto il Push da VS Code:
1.  Vai sul sito di GitHub nella scheda **Pull Requests**.
2.  Clicca su **"Compare & pull request"**.
3.  Assegna un compagno come **Reviewer**.
4.  **Regola d'oro:** Non fare mai il "Merge" (unione) da solo. Aspetta che un compagno legga il codice, lasci un commento o un "Approve".

---

## 🛠️ Regole per lo Sviluppo Hardware

Dato che lavoriamo con sensori e motori, seguiamo queste norme:

### 📍 Gestione dei Pin
Ogni volta che cambiate un collegamento sull'ESP32, aggiornate la tabella nel file `README.md` o nella **Wiki**. Non vogliamo bruciare sensori perché qualcuno ha cambiato un GPIO nel codice senza dirlo.

### 🧪 Test Prima dei Commit
Prima di fare il commit, assicuratevi che il codice **compili correttamente**. Se caricate codice che non compila, bloccate il lavoro di tutti gli altri.

### 🤫 Sicurezza (Password WiFi)
* Usate un file chiamato `secrets.h` per le credenziali del drone.
* Questo file è nel nostro `.gitignore`, quindi Git non lo caricherà online.
* Se crei nuove variabili segrete, avvisa il team!

---

## 📂 Organizzazione delle Cartelle
* `/src`: Codice sorgente dell'ESP32.
* `/docs`: Schemi elettrici, foto del setup e grafici dei test.
* `/lib`: Librerie specifiche per i sensori HC-SR04 o simili.

---

## 🆘 Cosa fare se esplode tutto?
Se su VS Code appaiono dei simboli rossi o scritte come "Merge Conflict":
1.  **Non farti prendere dal panico.** VS Code ti mostrerà le due versioni del codice (la tua e quella del tuo compagno).
2.  Scegli quella corretta cliccando su "Accept Incoming" o "Accept Current".
3.  Se non sei sicuro, **chiedi aiuto** prima di salvare. Meglio un'ora di ritardo che un drone che decolla al contrario.
