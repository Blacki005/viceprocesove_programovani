#include "proj2.h"

//README: Veskery popis funkce jako celku je obsazen v proj2.h.

void enter_for_service (struct process_t identity, FILE *output_file) {
    //pri vstupu do fce uz je zarazen do waiting - nemuze se stat ze by urednik odesel domu aniz by ho obslouzil

    //nahodne vybira sluzbu: cislo 1 az 3
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec * t.tv_sec);
    identity.service = (rand() % 3) + 1;

    sem_wait(mutex_write);
    fprintf_flush(output_file, "%u: Z %u: entering office for a service %u\n", ++(*A),identity.id, identity.service);
    sem_post(mutex_write);

    //zaradi se do fronty X a ceka na zavolani urednikem. Pote co se zaradil do fronty uz muze byt posta zavrena
    switch (identity.service) {
        case 1:
            sem_wait(service1);
            break;
        case 2:
            sem_wait(service2);
            break;
        case 3:
            sem_wait(service3);
            break;
    }
    sem_post(close_rdy);

    //muze byt zavolan az v momente, kdy je urednik pritomen na poste a nema prestavku
    sem_wait(officer_rdy);

    //ceka na vyrizeni sluzby (pokud zde dorazi moc brzo, zamkne z defaultu zamceny semafor podruhe)
    sem_wait(mutex_write);
    fprintf_flush(output_file, "%u: Z %u: called by office worker\n", ++(*A),identity.id);
    //printf_flush("There are %d waiting customers, size of officer rdy semaphore is: %d\n", *is_waiting, *(officer_rdy->__size));
    sem_post(mutex_write);

    usleep((rand() % 10)*1000);

    //odchazi domu po dokonceni sluzby
    sem_wait(mutex_write);
    fprintf_flush(output_file, "%u: Z %u: going home\n", ++(*A),identity.id);
    sem_post(mutex_write);

    return;
}