#ifndef PROJ2_H__
#define PROJ2_H__

#include "print.h"
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#define CUSTOMER 1
#define OFFICER 0
#define log fprintf_flush
#define IS_CHILD (getpid() != main_pid)

/**
 * @brief Struktura lokalni pro kazdy z procesu uchovavajici informace o nem.
 */
struct process_t {
    bool type; ///< Typ procesu - zakaznik / urednik.
    unsigned id; ///< ID procesu je jeho poradi vytvoreni. Je unikatni v ramci jeho typu.
    unsigned short service; ///< Sluzba, kterou proces jde vyridit / vyrizuje.
};

/// @brief Indikuje, jestli je otevreno (1) nebo zavreno (0).
extern int *is_open;
/// @brief A je poradi vypisovane hlasky. Inkrementuje se pri kazdem vypsani hlasky.
extern unsigned *A;
/// @brief Jeji hodnota je rovna poctu zakazniku, kteri vesli na postu a cekaji na obslouzeni.
extern int *is_waiting;


/// @brief Fronta na sluzbu 1.
extern sem_t *service1;
/// @brief Fronta na sluzbu 2.
extern sem_t *service2;
/// @brief Fronta na sluzbu 3.
extern sem_t *service3;
/// @brief Semafor chranici pristup ke sdilene promenne is_open.
extern sem_t *close_rdy;
/// @brief Semafor zabranujici kolizim mezi vice zamestnanci vybirajicimi sluzbu, kterou budou obsluhovat.
extern sem_t *choosing_service;
/// @brief Semafor zabranujici zavreni posty v momente, kdy se nejaky urednik rozhoduje zda si vzit prestavku.
/// @details Tato funkcionalita neni zajistena semaforem close_rdy, protoze semafor close_rdy zavira zakaznik v momente prichodu na postu a dochazelo by k uvaznuti. 
extern sem_t *taking_break;
/// @brief Semafor chranici pristup ke sdilenym promennym.
extern sem_t *mutex_write;
/// @brief Semafor obsahujici pocet uredniku, kteri momentalne neobsluhuji zakaznika a zaroven nemaji pauzu.
extern sem_t *officer_rdy;


/**
 * @brief Vytvori a inicializuje vsechny semafory.
 */
int init_semaphores();


/**
 * @brief Uvolni vsechny vytvorene semafory.
 */
void free_resources();


/**
 * @brief Funkce generuje procesy uredniku, kteri v ni pokracuji volanim funci sveho zivotniho cyklu.
 * @details V pripade chyby pri vytvareni ditete funkce vypise chybovou hlasku na stderr a pokracuje ve vytvareni deti.
 * @param [in] officers_cnt Pocet procesu uredniku, ktery ma byt vytvoren.
 * @param [in] max_break_time Maximalni cas prestavky v milisekundach.
 * @param [in] *output_file Soubor, do ktereho se maji vypisovat hlasky procesu.
 */
unsigned char create_officers(const unsigned officers_cnt, const unsigned max_break_time, FILE *output_file);


/**
 * @brief Funkce generuje procesy zakazniku, kteri v ni pokracuji volanim funci sveho zivotniho cyklu.
 * @details V pripade chyby pri vytvareni ditete funkce vypise chybovou hlasku na stderr a pokracuje ve vytvareni deti.
 * @param [in] customers_cnt Pocet procesu zakazniku, ktery ma byt vytvoren.
 * @param [in] max_wait_time Maximalni doba, kterou zakaznik po spusteni ceka, nez vejde na postu v milisekundach.
 * @param [in] *output_file Soubor, do ktereho se maji vypisovat hlasky procesu.
 */
unsigned char create_customers(const unsigned customers_cnt, const unsigned max_wait_time, FILE *output_file);

/**
 * @brief Po vstupu urednik vypisuje serving line of type x, ceka nahodny cas a uvolni jednoho zakaznika z vybrane rady.
 * @details Funkce dekrementuje semafor a snizuje sdilenou promennou indikujici celkovy pocet neobslouzenych zakazniku na poste.
 * @param [in] *service Semafor obsluhovane rady zakazniku.
 * @param [in] *output_file Soubor, do ktereho se maji vypisovat hlasky procesu.
 * @param [in] identity Identita procesu vstupujiciho do funkce.
 * @retval 1 Na sluzbu service nikdo neceka a neni tedy obslouzena.
 * @retval 0 Jeden zakaznik pozadujici sluzbu service byl uspesne obslouzen.
 */
unsigned char serve_line(sem_t *service, FILE *output_file, struct process_t identity);

/**
 * @brief Po vstupu do funkce vypise urednik hlasku taking break, ceka cas z intervalu <0, max_break_time>. Po ukonceni vypise break finished.
 * @details Zavira semafor taking_break.
 * @param [in] *output_file Soubor, do ktereho se maji vypisovat hlasky procesu.
 * @param [in] max_break_time Maximalni cas prestavky v milisekundach.
 * @param [in] identity Identita procesu vstupujiciho do funkce.
 */
void take_break(FILE *output_file, const unsigned max_break_time, struct process_t identity);

/**
 * @brief Zakaznik vejde na postu za ucelem vyrizeni nejake sluzby. Jeho obsluha je garantovana.
 * @details Po vstupu do funkce vypise proces svoje id a hlasku entering office. Nahodne vybere jednu ze sluzeb, zaradi se do prislusne rady a ceka na zavolani urednikem. Po zavolani ceka nahodny cas a odchazi domu.
 * @param [in] identity Identita procesu ktery do funkce vstupuje.
 * @param [in] *output_file Soubor, do ktereho se maji vypisovat hlasky procesu.
 */
void enter_for_service(struct process_t identity, FILE *output_file);

/**
 * @brief Kontroluje argumenty programu. Prevede je na cisla typu long, ulozi je do pole a vraci pointer na alokovane pole.
 * @param [in] argc Pocet argumentu.
 * @param [in] *argv Pole argumentu - stringu.
 * @return Pointer na dynamicky alokovane pole argumentu prevedenych na long.
 * @retval NULL Chyba alokace nebo neplatny argument.
 */
long *check_arguments(int argc, const char *argv[]);

/**
 * @brief Ceka nahodny cas v intervalu <max_shift_time / 2 , max_shift_time>. Pote zavre postu a vypise A: closing do souboru.
 * @param [in] max_shift_time Maximalni oteviraci doba v milisekundach.
 * @param [in] *output_file Soubor, do ktereho se ma vypsat hlaska A: closing.
 */
void close_office(unsigned max_shift_time, FILE *output_file);

#endif //PROJ2_H__