# Víceprocesové programování

## Program × proces

- **Program** = posloupnost instrukcí a dat uložená na disku (pasivní)
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
- prevence/vyhýbání se
- detekce a zotavení 
  - nechat deadlock vzinkout, pravidelně ho detekovat a následně řešit
  - systém "násilně" ukončí jeden nebo více procesů často za cenu ztráty práce jednoho z procesů

## Úkoly

### Úkol 1

- naprogramujte multiprocesový program v C++, kde může dojít k deadlocku

### Úkol 2

- opravte program tak, aby k deadlocku nikdy nemohlo dojít

### Úkol 3

- pomocí `mmap` vytvořte sdílenou paměť
- procesy do ní zapisují své PID
- zápis nesmí probíhat současně

### Úkol 4

##### Popis Úlohy (Pošta)

V systému máme 3 typy procesů: (0) hlavní proces, (1) poštovní úředník a (2) zákazník. Každý zákazník jde na poštu vyřídit jeden ze tří typů požadavků: listovní služby, balíky, peněžní služby. Každý požadavek je jednoznačně identifikován číslem (dopisy:1, balíky:2, peněžní služby:3). Po příchodu se zařadí do fronty dle činnosti, kterou jde vyřídit. Každý úředník obsluhuje všechny fronty (vybírá pokaždé náhodně jednu z front). Pokud aktuálně nečeká žádný zákazník, tak si úředník bere krátkou přestávku. Po uzavření pošty úředníci dokončí obsluhu všech zákazníků ve frontě a po vyprázdnění všech front odhází domů. Případní zákazníci, kteří přijdou po uzavření pošty, odcházejí domů.

##### Podrobná specifikace zadání

**Spuštění:**

```bash
$ ./proj2 NZ NU TZ TU F
```

- `NZ`: počet zákazníků
- `NU`: počet úředníků
- `TZ`: Maximální čas v milisekundách, po který zákazník po svém vytvoření čeká, než vejde na poštu (eventuálně odchází s nepořízenou). 0<=TZ<=10000
- `TU`: Maximální délka přestávky úředníka v milisekundách. 0<=TU<=100
- `F`: Maximální čas v milisekundách, po kterém je uzavřena pošta pro nově příchozí. 0<=F<=10000

**Chybové stavy:**

- Pokud některý ze vstupů nebude odpovídat očekávanému formátu nebo bude mimo povolený rozsah, program vytiskne chybové hlášení na standardní chybový výstup, uvolní všechny dosud alokované zdroje a ukončí se s kódem (exit code) 1.
- Pokud selže některá z operací se semafory, nebo sdílenou pamětí, postupujte stejně--program vytiskne chybové hlášení na standardní chybový výstup, uvolní všechny dosud alokované zdroje a ukončí se s kódem (exit code) 1.

**Implementační detaily:**

- Každý proces vykonává své akce a současně zapisuje informace o akcích do souboru s názvem `proj2.out`. Součástí výstupních informací o akci je pořadové číslo A prováděné akce (viz popis výstupů). Akce se číslují od jedničky.
- Použijte sdílenou paměť pro implementaci čítače akcí a sdílených proměnných nutných pro synchronizaci.
- Použijte semafory pro synchronizaci procesů.
- Nepoužívejte aktivní čekání (včetně cyklického časového uspání procesu) pro účely synchronizace.
- Pracujte s procesy, ne s vlákny.

##### Hlavní proces

- Hlavní proces vytváří ihned po spuštění NZ procesů zákazníků a NU procesů úředníků.
- Čeká pomocí volání `usleep` náhodný čas v intervalu `<F/2,F>`
- Vypíše: `A: closing`
- Poté čeká na ukončení všech procesů, které aplikace vytváří. Jakmile jsou tyto procesy ukončeny, ukončí se i hlavní proces s kódem (exit code) 0.

##### Proces Zákazník

- Každý zákazník je jednoznačně identifikován číslem `idZ`, `0<idZ<=NZ`
- Po spuštění vypíše: `A: Z idZ: started`
- Následně čeká pomocí volání `usleep` náhodný čas v intervalu `<0,TZ>`
- Pokud je pošta uzavřena
  - Vypíše: `A: Z idZ: going home`
  - Proces končí
- Pokud je pošta otevřená, náhodně vybere činnost X---číslo z intervalu `<1,3>`
  - Vypíše: `A: Z idZ: entering office for a service X`
  - Zařadí se do fronty X a čeká na zavolání úředníkem.
  - Vypíše: `Z idZ: called by office worker`
  - Následně čeká pomocí volání `usleep` náhodný čas v intervalu `<0,10>` (synchronizace s úředníkem na dokončení žádosti není vyžadována).
  - Vypíše: `A: Z idZ: going home`
  - Proces končí

##### Proces Úředník

- Každý úředník je jednoznačně identifikován číslem `idU`, `0<idU<=NU`
- Po spuštění vypíše: `A: U idU: started`
- **[začátek cyklu]**
- Úředník jde obsloužit zákazníka z fronty X (vybere náhodně libovolnou neprázdnou).
  - Vypíše: `A: U idU: serving a service of type X`
  - Následně čeká pomocí volání `usleep` náhodný čas v intervalu `<0,10>`
  - Vypíše: `A: U idU: service finished`
  - Pokračuje na **[začátek cyklu]**
- Pokud v žádné frontě nečeká zákazník a pošta je otevřená vypíše
  - Vypíše: `A: U idU: taking break`
  - Následně čeká pomocí volání `usleep` náhodný čas v intervalu `<0,TU>`
  - Vypíše: `A: U idU: break finished`
  - Pokračuje na **[začátek cyklu]**
- Pokud v žádné frontě nečeká zákazník a pošta je zavřená
  - Vypíše: `A: U idU: going home`
  - Proces končí

##### Obecné informace

- Projekt implementujte v jazyce C.
- Kontrolujte, zda se všechny procesy ukončují korektně a zda při ukončování správně uvolňujete všechny alokované zdroje.
- Poznámka k testování: Můžete si nasimulovat častější přepínání procesů například vložením krátkého uspání po uvolnění semaforů apod.

##### Překlad

- Pro překlad používejte nástroj make.
- Překlad se provede příkazem `make` v adresáři, kde je umístěn soubor Makefile.
- Po překladu vznikne spustitelný soubor se jménem `proj2`, který bude umístěn ve stejném adresáři jako soubor Makefile
- Spustitelný soubor může být závislý pouze na systémových knihovnách---nesmí předpokládat existenci žádného dalšího studentem vytvořeného souboru (např. spustitelný soubor úředník, konfigurační soubor, dynamická knihovna zákazník, ...).
- Zdrojové kódy překládejte s přepínači `-std=gnu99 -Wall -Wextra -Werror -pedantic`
- Pokud to vaše řešení vyžaduje, lze přidat další přepínače pro linker (např. kvůli semaforům či sdílené paměti, `-pthread`, `-lrt`, . . . ).

##### Příklad výstupu

Příklad výstupního souboru `proj2.out` pro následující příkaz:

```bash
$ ./proj2 3 2 100 100 100
```

```
1: U 1: started
2: Z 3: started
3: Z 1: started
4: Z 1: entering office for a service 2
5: U 2: started
6: Z 2: started
7: Z 3: entering office for a service 1
8: Z 1: called by office worker
9: U 1: serving a service of type 2
10: U 1: service finished
11: Z 1: going home
12: Z 3: called by office worker
13: U 2: serving a service of type 1
14: U 1: taking break
15: closing
16: U 1: break finished
17: U 1: going home
18: Z 2: going home
19: U 2: service finished
20: U 2: going home
21: Z 3: going home
```

## Zdroje

- https://cw.fel.cvut.cz/old/_media/courses/a0b36pr2/lectures/07/36pr2-07_vlakna_uvod.pdf
- *Little Book of Semaphores*, Allen B. Downey
- předmět Operační systémy na VUT FIT

