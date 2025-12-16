# Víceprocesové programování

## Program × proces

- **Program** = posloupnost instrukcí a data uložená na disku (pasivní)
- **Proces** = abstrakce spuštěného programu, včetně aktuálních hodnot registrů a proměnných
  - Příklad: stavím sídliště
    - nákres domu = program
    - postavené domy = procesy
  - Mohu spustit dvě instance editoru → jeden program, dva procesy

- Operační systém určuje, jaký proces poběží
  - Veškerý běžící software je množina sekvenčně běžících procesů

## Stavy procesu

- běžící
- čeká na nějakou událost
- pozastavený (čeká na CPU time)
- ukončen

## Proces × vlákno

- **Proces**:
  - alokuje prostředky: adresový prostor, data, zásobník, otevřené soubory, potomky

- **Vlákno**:
  - jeden proces může mít více vláken
  - vlákna sdílí paměťový prostor procesu
  - vlákno má vlastní:
    - registry
    - zásobník
    - lokální proměnné
  - sdílené mezi vlákny:
    - paměť
    - otevřené soubory
    - potomci

- Procesy se implicitně spouští s jedním vláknem, které může vytvářet další vlákna

## Ukázka (Linux)

```bash
g++ chat_server.cpp -o chat_server
./chat_server
```

### Zobrazení vláken – interaktivně

```bash
top -H -p $(pgrep chat_server)
```

- `-H` = zobrazí vlákna místo procesů
- každé vlákno jako samostatný řádek (PID je ve skutečnosti Thread ID)
- stejný název procesu → vlákna patří jednomu procesu
- důležité sloupce:
  - `S` = stav (`R` = běžící, `S` = čeká)
  - `VIRT / RES / SHR` = stejné pro všechna vlákna (sdílená paměť)
  - `%CPU` = může se lišit

### Zobrazení vláken – staticky

```bash
ps -T -p $(pgrep chat_server)
```

- `PID` = ID hlavního procesu
- `SPID` = Thread ID (unikátní pro každé vlákno)

## Windows ekvivalenty

```cmd
tasklist /FI "IMAGENAME eq chat_server.exe"
```

```cmd
wmic process where name="chat_server.exe" get ProcessId,ThreadCount
```

- **Process Explorer (Sysinternals)**
  - View → Show Lower Pane → Threads
  - zobrazuje TID pro každé vlákno

- **PowerShell**:

```powershell
(Get-Process chat_server).Threads
```

## Pojmy

### Kritická sekce

- část programu, kde procesy používají sdílenou paměť nebo proměnnou

### Sdílená paměť

- proces běžně nemůže čekat na data v paměti jiného procesu (protože do ní nevidí)
- aby mohly procesy komunikovat mezi sebou, musíme explicitně vyžádat sdílenou paměť od OS
- rizika jako race condition, deadlock, starvation,..

- alokace sdílené paměti:
  - `shmget`
  - `mmap`
- mapujeme dva logické paměťové prostory na jednu fyzickou adresu

### Semafor

- dvojice:
  - celočíselná proměnná `N`
  - fronta procesů

- význam `N`:
  - `N > 0` → proces pokračuje
  - `N == 0` → PID uložen do fronty

- semafor nemusí vybírat první proces z fronty
- atomické operace:
  - `lock`
  - `unlock`
  - inkrementace / dekrementace `N`

```c
sem_open
sem_wait  // lock
sem_post  // unlock
```

### Monitory

- abstrakce (podobná třídě) nad sdílenou pamětí
- definované operace
- ke sdíleným datům lze přistupovat pouze přes monitor
- uvnitř monitoru nelze čekat na jiný proces → vylučuje deadlock

## Problémy při práci s procesy

### Deadlock

- situace, kdy existuje neprázdná množina procesů, kde pro každý platí:
  - je pozastaven
  - čeká na uvolnění zdroje s výlučným přístupem
  - čeká neomezeně dlouho
- zdroj vlastní jiný proces z této množiny
- vzniká cyklická závislost

### Časově závislá chyba (race condition)

- dva nebo více procesů zapisují do sdílené paměti

### Starvation

- proces může teoreticky pokračovat, ale je neustále předbíhán jinými procesy

## Jak problémy řešit

- definice kritické sekce
- vzájemné vylučování
  - znemožňuje, aby se více procesů nacházelo současně v kritické sekci

## Úkoly

### Úkol 1

- naprogramujte multiprocesový program v C++, kde dojde k deadlocku

### Úkol 2

- opravte program tak, aby k deadlocku nikdy nemohlo dojít

### Úkol 3

- pomocí `mmap` vytvořte sdílenou paměť
- procesy do ní zapisují své PID
- zápis nesmí probíhat současně

### Úkol 4

- **Post office / Barbershop problem**
- viz PDF

## Zdroje

- https://cw.fel.cvut.cz/old/_media/courses/a0b36pr2/lectures/07/36pr2-07_vlakna_uvod.pdf
- *Little Book of Semaphores*, Allen B. Downey

