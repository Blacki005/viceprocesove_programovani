#include "proj2.h"

//README: Veskery popis funkce jako celku je obsazen v proj2.h.

void close_office(unsigned max_shift_time, FILE *output_file) {
    //nahodny seed s vysokou presnosti
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec * t.tv_sec);

    //rand() % 0 nelze, proto se musi odlisovat pripady kdy max_shift_time > 1 a max_shift_time <= 1
    if (max_shift_time > 1) {
        //cekani nahodny cas
        usleep(((max_shift_time / 2) + (rand() % (max_shift_time / 2))) * 1000 );

        //nastaveni sdilene promenne is_open je kriticka sekce - nesmi se stat ze by novy zakaznik prichazel nebo se urednik rozhodoval zda si vezme pauzu
        sem_wait(close_rdy);
        sem_wait(taking_break);

        sem_wait(mutex_write);
        fprintf_flush(output_file, "%u: closing\n", ++(*A));
        sem_post(mutex_write);

    } else {
        //pokud je max_shift_time <= 1, pak je v intervalu <F/2, F> pouze max_shift_time
        usleep(max_shift_time * 1000);

        //cekani na stejnych semaforech jako v podmince vyse
        sem_wait(close_rdy);
        sem_wait(taking_break);

        sem_wait(mutex_write);
        fprintf_flush(output_file, "%u: closing\n", *A);
        sem_post(mutex_write);
    }

    sem_wait(mutex_write);
    *is_open = 0;
    sem_post(mutex_write);

    //uvolneni semaforu
    sem_post(taking_break);
    sem_post(close_rdy);

    return;
}