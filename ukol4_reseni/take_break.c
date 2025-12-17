#include "proj2.h"

//README: Veskery popis funkce jako celku je obsazen v proj2.h.

void take_break(FILE *output_file, const unsigned max_break_time, struct process_t identity) {

    sem_wait(mutex_write);
    fprintf_flush(output_file, "%u: U %u: taking break\n", ++(*A),identity.id);
    sem_post(mutex_write);

    sem_post(taking_break);

    //pokud neni maximalni cas prestavky 0, generuje se seed a ceka se nahodny cas.
    if (max_break_time != 0) {
        //genrovani seedu je zavisle na aktualnim case v mikrosekundach
        struct timeval t;
        gettimeofday(&t, NULL);
        srand(t.tv_usec * t.tv_sec);
        usleep((rand() % max_break_time) * 1000);
    }
    
    //semafor zustava po odchodu z funkce zavren, protoze proces pokracuje na zacatek cyklu, kde vyhodnocuje sdilene promenne - odemkne se az po jejich vyhodnoceni
    sem_wait(mutex_write);
    fprintf_flush(output_file, "%u: U %u: break finished\n", ++(*A),identity.id);

    return;
}