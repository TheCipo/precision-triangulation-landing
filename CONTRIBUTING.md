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

## 🔄 Flusso di Sviluppo: Issue → Branch → PR

Per portare nuove implementazioni nel codice, seguiamo questo flusso rigoroso per mantenere ordine e qualità:

### 1. **Crea un'Issue**
   - Vai su GitHub e apri una nuova Issue nella scheda **Issues**.
   - Descrivi chiaramente il compito: cosa implementare, perché, e eventuali dettagli tecnici (es: "Implementare comunicazione WiFi con DJI Tello per comandi di volo").
   - Assegna etichette appropriate (es: `enhancement`, `bug`) e collegala al progetto se necessario.
   - Discuti con il team per confermare l'approccio.

### 2. **Crea un Branch collegato all'Issue**
   - In VS Code, crea un nuovo branch dal `main` (vedi sezione sotto).
   - Nomina il branch con il numero dell'Issue (es: `issue-11-wifi-tello` o `feature-issue-11`).
   - Questo collega automaticamente il branch all'Issue su GitHub.

### 3. **Implementa e Testa**
   - Scrivi il codice seguendo la struttura modulare (vedi sotto).
   - Compila e testa prima di committare.
   - Fai commit frequenti con messaggi chiari.

### 4. **Apri una Pull Request (PR)**
   - Dopo il push, vai su GitHub e crea una PR dal tuo branch verso `main`.
   - Descrivi le modifiche e collega l'Issue (es: "Closes #11").
   - Assegna reviewer e aspetta l'approvazione prima del merge.
   - **Regola:** Nessun merge senza almeno un'approvazione.

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

## 📋 Struttura del Codice nei File

Il codice è organizzato in moduli separati per facilitare la manutenzione e lo sviluppo. Ogni modulo ha un file `.cpp` per l'implementazione e un `.h` per le dichiarazioni. Ecco una panoramica breve:

* **`esp32.ino`**: File principale Arduino per ESP32. Gestisce il setup, loop e orchestrazione dei moduli.
* **`sensor.cpp/h`**: Gestione dei sensori HC-SR04. Lettura distanze, validazione e timeout.
* **`filter.cpp/h`**: Filtraggio dati con mediana su 21 campioni per ridurre rumore.
* **`positioning.cpp/h`**: Calcolo trilaterazione per determinare coordinate (x, y) del drone.
* **`setup.cpp/h`**: Configurazione runtime e calibrazione sensori (menu seriale, storage flash).
* **`comunication.cpp/h`**: Interfaccia seriale per input/output (lettura linee, debug).
* **`drone.cpp/h`**: Placeholder per controllo drone DJI Tello (WiFi, comandi volo).
* **`centering_control.cpp/h`**: Placeholder per logica di centraggio e atterraggio.
* **`config.h`**: Costanti globali (pin GPIO, limiti distanze, numero campioni).
* **`secrets.h`**: Credenziali segrete (WiFi, non committato).

Ogni nuovo pezzo di codice deve seguire questa struttura modulare: separa logica in funzioni, usa header per dichiarazioni, e mantieni dipendenze chiare.

---

## 🛠️ Come Implementare Nuove Funzionalità

Quando aggiungi una nuova feature:
1. **Pianifica**: Discuti nell'Issue e decidi in quale modulo inserire (o creane uno nuovo se necessario).
2. **Struttura**: Ogni funzione deve avere un header chiaro, commenti, e validazione input.
3. **Testa**: Verifica che non rompa il resto (compila, loop funziona).
4. **Documenta**: Aggiorna README o Wiki con la nuova logica.
5. **Commit**: Messaggi specifici (es: "Implementato invio comandi UDP a Tello").

Per nuovi moduli: Crea coppia `.cpp/.h`, includi nel `esp32.ino`, e aggiorna `config.h` se servono nuove costanti.

---

## 🔄 Il Flusso delle Pull Request (Revisione)

Una volta fatto il Push da VS Code:
1.  Vai sul sito di GitHub nella scheda **Pull Requests**.
2.  Clicca su **"Compare & pull request"**.
3.  Assegna un compagno come **Reviewer**.
4.  **Regola d'oro:** Non fare mai il "Merge" (unione) da solo. Aspetta che un compagno legga il codice, lasci un commento o un "Approve". Ogni PR deve essere autorizzata da almeno un reviewer prima del merge.

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
