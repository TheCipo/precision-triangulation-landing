# Relazione Tecnica e Ragionamento Progettuale

*Sistema di Trilaterazione a Ultrasuoni per il Posizionamento Autonomo e l'Atterraggio di Precisione di un DJI Tello EDU (ESP32 + HC-SR04)*

Questo documento ricostruisce passo dopo passo non solo le modifiche apportate al codice sorgente, ma soprattutto il ragionamento fisico, matematico e informatico che ha portato a ciascuna scelta progettuale durante i test sperimentali. L'obiettivo didattico del progetto è guidare un DJI Tello EDU a convergere iterativamente verso l'origine (0, 0) — il centro di un triangolo equilatero di raggio R =51 cm ai cui vertici sono posti tre sensori HC-SR04 — mantenendo una tolleranza stretta (acceptedDistanceError = 5 cm).

## 1. Estensione del Range Utile: il legame fisico tra MAXtimeout e MAXdistance

### Il problema osservato nei primi test

Nei primi voli di prova, il sistema sembrava affidarsi quasi esclusivamente a un solo sensore (il Sensore A), entrando continuamente in modalità degradata (degradedMode) anche se tutti e tre i sensori HC-SR04 funzionavano correttamente se testati singolarmente.

### Il ragionamento fisico e geometrico

Per capire perché due sensori su tre restituissero costantemente ND (-9999), abbiamo analizzato due parametri che lavorano in cascata dentro la funzione readDistance() in sensor.cpp:

* **Il limite temporale dell'impulso (MAXtimeout = 10.000 µs in config.h):** La funzione pulseIn(echoPin, HIGH, MAXtimeout) attende il ritorno dell'onda sonora. Poiché il suono viaggia nell'aria a circa 343 m/s (0,0343 cm/µs) e deve percorrere la distanza due volte (andata verso il drone e ritorno verso il ricevitore), 10.000 µs corrispondono a una distanza fisica massima di:
d_max = (10.000 µs × 0,0343 cm/µs) / 2 ≈ 171 cm.
Se l'eco impiega più di 10 ms a tornare (o arriva attenuata perché la scocca curva del Tello riflette meno di una parete piatta), pulseIn() scade e restituisce 0 (quindi ND).
* **Il filtro software successivo (MAXdistance = 80 cm in setup.cpp e in Flash):** Anche quando pulseIn() riusciva a misurare l'eco entro i 10.000 µs, la riga if (dist > MAXdistance) return ND; scartava qualsiasi misura superiore a 80 cm. Tuttavia, i tre sensori sono posti ai vertici di un triangolo equilatero di raggio R = 50–51 cm dal centro, con una distanza tra due vertici pari a L = R√3 ≈ 86,6 cm. Quando il drone si trova al centro (0, 0) a quota Z = 30–50 cm, la distanza obliqua 3D è già √(50² + 40²) ≈ 64 cm. Appena il drone si sposta di soli 15–20 cm verso il Sensore A (d_A ≈ 35 cm), la distanza dagli altri due sensori B e C sale immediatamente a 85–105 cm: superando la soglia di 80 cm, B e C venivano tagliati come ND! *(Nota: questo è vero in parte poiché all’IA non è stato detto che i sensori si trovano a circa 25 cm di altezza e quindi ha dato per scontato fossero al livello del suolo.)*
* **L'effetto domino nel loop():** In positioning.cpp, la trilaterazione richiede tutti e tre i sensori validi (if (d0 == ND || d1 == ND || d2 == ND) return;). Appena B o C superavano gli 80 cm, il sistema entrava in degradedMode(), dove un ciclo for (int i = 0; i < 3; i++) cercava il primo sensore non-ND e faceva break: trovando sempre i = 0 (Sensore A), ignorava sistematicamente gli altri due.

### La soluzione adottata (config.h, setup.cpp, sensor.cpp)

Abbiamo portato MAXtimeout a 18.000 µs (circa 300 cm di portata fisica massima per evitare tagli hardware) e innalzato MAXdistance a 140 cm (sovrascrivendo il vecchio valore 80 salvato nella memoria Flash/Preferences dell'ESP32 tramite l'opzione 2 del menu seriale), affinando inoltre la conversione temporale in duration / 58 (≈ 343 m/s).

## 2. Campionamento Alternato (A → B → C) e Pulizia delle Variabili

### Il ragionamento acustico e informatico

Analizzando la routine di acquisizione nel file principale (.ino), sono emersi due problemi distinti:

* **Echi residui e cross-talk acustico:** Originariamente, il codice raccoglieva 5 campioni consecutivi sullo stesso sensore (5 ping di fila su A, poi 5 su B, poi 5 su C). In ambiente chiuso, le onde ultrasoniche rimbalzano su pavimento, pareti e supporto del drone; interrogare lo stesso sensore a raffica fa sì che l'eco tardiva del primo ping venga letta come distanza ravvicinata nel ping successivo. Invertendo i due cicli for (ciclo esterno sui 5 campioni j, ciclo interno sui 3 sensori i con pausa di 60 ms tra un sensore e l'altro), si ottiene la sequenza A → B → C: in questo modo ogni singolo sensore beneficia di circa 180 ms di silenzio acustico prima di emettere un nuovo impulso.
* **Shadowing della variabile globale finalDistances:** All'inizio del loop(), l'istruzione int finalDistances[3] = {ND, ND, ND}; ridichiarava un nuovo array locale omonimo, oscurando l'array globale dichiarato in config.h e lasciandolo non inizializzato per eventuali funzioni esterne. È stata sostituita con un ciclo for che azzera direttamente gli elementi dell'array globale.

## 3. Proiezione Trigonometrica dell'Inclinazione dei Sensori (sensorAngle)

### Il ragionamento geometrico

I tre sensori HC-SR04 non sono montati rasoterra in piano, ma su supporti orientabili inclinati verso l'alto (es. 20° o 45° rispetto all'orizzontale) per puntare al volume di volo del drone. Di conseguenza, la distanza misurata dal sensore è l'ipotenusa (distanza obliqua d_grezza) di un triangolo rettangolo, mentre le formule di trilaterazione operano sulle coordinate (x, y) proiettate sul piano orizzontale.

Senza proiezione, anche quando il drone si trova esattamente sopra il centro (0, 0), la distanza obliqua letta risulta maggiore del raggio R del triangolo (es. 55–58 cm invece di 51 cm), falsando il calcolo della posizione soprattutto quando uno dei sensori viene a mancare. Introducendo il parametro configurabile sensorAngle (in gradi, con convenzione 0° = orizzontale e 90° = verticale) salvato in Flash, sensor.cpp verifica prima che la distanza obliqua rientri in [MINdistance, MAXdistance] e poi la proietta sul piano XY tramite il coseno: *(Nota: valutare se modificare il codice, inserendo anche l’altezza a cui si trovano i sensori, anche se la formula usata per il calcolo dell’ipotenusa non tiene conto dell’altezza del triangolo rettangolo costruito usando il raggio R come base)*

```cpp
float rad = sensorAngle * PI / 180.0f;
int horizontalDist = (int)round(rawDist * cos(rad));

```

## 4. Attesa Attiva delle Risposte UDP in Tello.cpp: perché i delay(5000) erano dannosi

### Cosa succedeva realmente nella libreria originale

Nella versione originale di Tello::sendCommand(), subito dopo aver spedito il pacchetto UDP con udpclient->endPacket(), il codice chiamava immediatamente udpclient->parsePacket() e read() senza alcuna attesa. Dal punto di vista fisico e di rete, un microsecondo dopo l'invio il pacchetto Wi-Fi non è nemmeno arrivato al drone, e tantomeno il Tello ha avuto il tempo di eseguire lo spostamento e rispondere "ok". Di conseguenza, parsePacket() restituiva sempre 0, il buffer restava vuoto e tutti i metodi (drone.go(), drone.takeoff(), ecc.) restituivano sempre false. *In realtà nella pratica restituivano True in modo sfalsato.*

### Perché i delay(5000) fissi erano più dannosi che utili

Per compensare l'assenza di lettura della risposta, nello sketch originale erano stati inseriti dei delay(5000) bloccanti dopo ogni comando. Questo approccio causava due problemi opposti e ugualmente gravi:

* **Sovrapposizione e perdita di comandi (ritardo troppo breve):** Se una manovra complessa o un riassestamento richiedeva più di 5 secondi, l'ESP32 inviava il comando successivo mentre il drone era ancora in movimento. Il firmware del Tello EDU scarta i nuovi comandi di movimento mentre ne sta eseguendo uno (rispondendo "error" o ignorandoli).
* **Deriva in aria e tempi morti (ritardo troppo lungo):** Per un micro-spostamento di 20 cm che il drone completa in circa 1,2 secondi, l'ESP32 restava congelato per altri 3,8 secondi inutili. In quel lasso di tempo le correnti d'aria indoor spostavano il drone fuori dal punto appena raggiunto, e l'intera iterazione (2-4 comandi) impiegava 15–20 secondi.
* **Bug logico in Tello::init():** La condizione originale while(!response.compare("ok")) era invertita: in C++, string::compare() restituisce 0 quando due stringhe sono identiche, quindi !0 diventava vero proprio quando il drone rispondeva "ok".

### La soluzione adottata (Attesa attiva non bloccante + lastResponse)

In Tello.cpp abbiamo introdotto uno svuotamento preventivo del buffer UDP (while(udpclient->parsePacket() > 0) flush();) per eliminare pacchetti vecchi, seguito da un ciclo di attesa attiva basato su millis() con passo di 10 ms (che lascia respirare lo stack Wi-Fi in background dell'ESP32). Appena il Tello risponde "ok" o "error", la funzione ritorna istantaneamente senza attese superflue. Inoltre, abbiamo aggiunto la variabile pubblica lastResponse in Tello.h per distinguere il motivo esatto di un fallimento (es. "No valid imu" vs "out of range").

## 5. Posizionamento Ibrido: Bilaterazione 2D, Cono Radiale da 15° e Filtri di Plausibilità

### Il ragionamento geometrico: perché passare da 3 → 2 → 1

* **Bilaterazione 2D (2 sensori validi su 3):** Quando un sensore perde momentaneamente l'eco ma gli altri due leggono correttamente, non serve ripiegare su un solo sensore. Geometricamente, due circonferenze centrare nei due sensori attivi si intersecano in due punti simmetrici rispetto alla congiungente. Poiché il drone vola all'interno del triangolo, la funzione calculatePosition2Sensors() calcola entrambe le intersezioni e seleziona automaticamente quella più vicina all'origine (0, 0), consentendo di continuare il centraggio 2D.
* **Con 1 solo sensore attivo (Cono radiale da ...):** Sfruttando il cono di emissione stretto da ... puntato verso il centro, si stima la posizione lungo la direttrice radiale del sensore attivo tramite le proporzioni vettoriali:
* **Filtro di plausibilità fisica e anti-teletrasporto (isPositionPlausible):** Durante il volo, doppi rimbalzi acustici sul pavimento possono far leggere simultaneamente a due sensori valori raddoppiati (es. 101 cm e 107 cm nel log 4), generando una coordinata calcolata a 68 cm dal centro. Poiché i sensori sono posti su un cerchio di raggio R = 51 cm, una posizione oltre i 55 cm dall'origine è fisicamente impossibile dentro l'area di misura, così come un salto istantaneo superiore a 35 cm tra due letture consecutive. La funzione isPositionPlausible() scarta qualsiasi coordinate fuori dal raggio dinamico (...) e blocca I salti istantanei > 35 cm entro 4 secondi dall'ultima misura valida (millis() - lastValidTime < 4000) evitando anche i falsi scarti subito dopo un autorecupero.

## 6. Controllo Proporzionale Smorzato (Kp = 0.75) e Micro-Manovre Sicure

### Il ragionamento sulla dinamica di volo

Per soddisfare il requisito didattico di mantenere acceptedDistanceError = 5 cm (convergenza iterativa ad alta precisione senza accontentarsi di un atterraggio approssimativo a 15 cm), il controllo originale presentava tre difetti:

* **Effetto pendolo (sovracorrezione con Kp = 1.0):** Chiedendo al drone di spostarsi esattamente del 100% dell'errore misurato (es. 7 cm), tra inerzia e tolleranza degli ultrasuoni (±1–2 cm) il drone superava il centro finendo a -8 cm dal lato opposto. Applicando un guadagno proporzionale smorzato Kp = 0.75 (75% dell'errore, con passo minimo di 2 cm e tetto massimo di sicurezza a ±25 cm), il drone si avvicina asintoticamente a (0, 0) senza mai oscillare.
* **Allineamento assi Sensori → Tello SDK e traiettoria diagonale go():** Con Sensore A frontale (0, +50) e Sensore B a destra (+43.3, -25), l'asse +Y dei sensori corrisponde all'avanzamento (+telloX), mentre l'asse +X dei sensori (destra) corrisponde a -telloY (poiché nel SDK Tello Y positivo è sinistra). L'uso del comando unico diagonale drone.go(telloX, telloY, 0, 20) a velocità ridotta (20 cm/s) sostituisce i 4 scatti separati (right/left/forward/back), riducendo lo stress sull'odometria visiva.
* **Micro-manovra a 2 passi con offset adattativo verso l'interno, rientro condizionato e compensazione della retromarcia:** Il comando SDK go x y z speed rifiuta con "out of range" qualsiasi spostamento in cui tutti gli assi abbiano modulo inferiore a 20 cm. Nei primi test, sommando sempre +20 cm, se telloX era negativo (es. -9 cm) si otteneva 20 + (-9) = 11 cm (< 20 cm): il primo comando falliva, ma il codice eseguiva comunque il rientro go(-20, 0), facendo arretrare il drone a vuoto. La versione attuale sceglie un offset di ±20 cm orientato sempre verso l'interno del triangolo (arretra di -20 cm se il drone è già nella metà anteriore Dy > 0, avanza di +20 cm se è nella metà posteriore Dy <= 0) ed esegue il Comando 2/2 di rientro SOLO SE safeGo() del Comando 1/2 restituisce true. Inoltre poiché il Tello EDU percorre meno strada all'indietro rispetto all'avanti a bassa quota, è stato introdotto il fattore moltiplicativo BACKWARD_COMPENSATION = 1.35 su tutti gli spostamenti negativi sull'asseX.

## 7. Sicurezze Operative: Recupero IMU, Auto-Recupero su Pad e Land Manuale

* **Procedura recoverImu() contro l'errore 'No valid imu':** Volando a quota ridotta (~30 cm dopo down 20), la telecamera ottica inferiore del Tello ha un campo visivo stretto e risente dell'effetto suolo, restituendo talvolta "error No valid imu". Quando safeGo() intercetta questa stringa in lastResponse, attiva recoverImu(): tenta una salita di 30 cm (up 30), esegue un micro-assestamento orizzontale (forward 20 → back 20) per riattivare il flusso ottico sul pavimento e ridiscende di 30 cm solo se la salita era effettivamente avvenuta, riprovando poi il comando originale.
* **Auto-recupero tramite Mission Pad m2 (resetTello()):** Sfruttando il sistema di coordinate assolute memorizzato dal Tello sul tappetino m2, se il drone resta fuori dal campo dei sensori per più di MAXERRORS (10) cicli consecutivi mentre è in volo (!landed), il sistema richiama resetTello(), che riporta automaticamente il drone sopra m2 (go 0 0 50 30 m2) e ripete l'ingresso nel triangolo.
* **Atterraggio manuale non bloccante (checkSerialLand()) e blocco post-atterraggio:** Scrivendo 'land' o 'l' nel Monitor Seriale in qualsiasi momento (anche durante il campionamento dei sensori), l'ESP32 invia immediatamente drone.land() e imposta landed = true. Posizionando if (checkSerialLand() || landed) return; come prima istruzione del loop(), si impedisce che il contatore errorCount continui a salire a drone ormai a terra.

## 8. Tabella Riepilogativa dei File Modificati

| File | Modifiche Principali | Obiettivo Funzionale |
| --- | --- | --- |
| **config.h** | MAXtimeout = 18000, aggiunta extern float sensorAngle; | Estende il range acustico a ... e condivide l'angolo dei supporti. |
| **setup.cpp** | Input e salvataggio in Flash di sensorAngle e MAXdistance = 140 | Permette di configurare raggio, inclinazione e soglie senza ricompilare. |
| **sensor.cpp** | Proiezione orizzontale round(rawDist * cos(rad)) | Converte la misura obliqua 3D nella distanza sul piano ... |
| **Tello.h / Tello.cpp** | Polling attivo UDP con millis(), pulizia buffer, lastResponse | Sincronizza i comandi Wi-Fi ed espone gli errori specifici del drone. |
| **positioning.h / .cpp** | Funzione calculatePosition2Sensors() | Calcola ... con 2 sensori scegliendo l'intersezione interna. |
| **drone.cpp** | ..., BACKWARD_COMPENSATION = 1.35, recoverImu(), safeGo() | Convergenza fluida entro ..., compensazione retromarcia e ripristino IMU. |
| **src.ino** | Lettura alternata ..., isPositionPlausible(), cono ..., checkSerialLand() | Elimina *cross-talk* e rimbalzi spuri, gestisce avvicinamento radiale e land manuale. |

## 9. TO DO list:

| Da fare | Obbiettivo Funzionale | Note |
| --- | --- | --- |
| **Portare i sensori dagli attuali 25 cm di altezza a 50 cm** | Permettere al Drone di operare alla suo quota di volo ideale di 50-60 cm, dove il cono di ricezione dei sensori sotto la pancia raggiunge la precisione massima | Da stampare a resina 3 supporti per ogni colonna – tempo di lavoro stimato: ca 5 giorni |
| **Creare una base di atterraggio più larga** | Permettere al drone di atterrare correttamente anche con la tolleranza di 5 cm rispetto al centro | Riprogettare e stampare l'intero blocco per alloggiare comodamente l'esp 32 e i relativi moduli montati su eventuale breadboard e nuova base di atterraggio più ampia – tempo di lavoro stimato: almeno 7-10 giorni |
| **Nuovi binari graduati** | Correggere errore sulla distanza riportata che al momento non è relativa al centro dei sensori, ma all'inizio della colonna porta sensori, con uno scarto di circa 2 cm | Ridisegnare nuova scala graduata corretta e stampare nuovi binari con incastro efficiente alla base centrale. Tempo di lavoro ca 5-7 giorni |
| **Sostituzione sensori ad ultrasuoni, con articolo nuovo mai usato e nuovo cablaggio con cavi più corti e crimpatti correttamente** | Ridurre eventuali interferenze e avere certezza di funzionamento corretto e costante di tutti e 3 i sensori. |  |
| **Uniformare colore del campo di volo** | Uniformare il colore dell'intero tavolo su cui facciamo volare il drone, eliminando il blocco marrone, e passando ad un bianco neutro opaco dovrebbe ridurre gli errori di misura del drone |  |
| **Varie ed eventuali** |  |  |