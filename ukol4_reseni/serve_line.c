#include "proj2.h"

//README: Veskery popis funkce jako celku je obsazen v proj2.h.

unsigned char serve_line(sem_t *service, FILE *output_file, struct process_t identity) {
    //pokud je fronta prazdna, vraci 1
    if (*(service->__size) > 0) {
        sem_post(choosing_service);//dalsi urednik muze jit obsluhovat - hodnoty ve frontach jsou stabilni
        return 1;
    }

    sem_wait(mutex_write);
    fprintf_flush(output_file, "%u: U %u: serving a service of type %u\n", ++(*A),identity.id, identity.service);
    sem_post(mutex_write);


    //ceka nahodny cas v intervalu <0,10> -- obsluhuje zakaznika
    usleep((rand() % 10) * 1000);
    sem_post(service);

    //zakaznik muze jit k prepazce
    sem_wait(mutex_write);
    if ((*is_waiting) > 0) {
        (*is_waiting) -= 1;
    }
    sem_post(mutex_write);

    //dalsi urednik muze jit obsluhovat - hodnoty ve frontach jsou stabilni
    sem_post(choosing_service);

    sem_wait(mutex_write);
    fprintf_flush(output_file, "%u: U %u: service finished\n", ++(*A), identity.id);
    sem_post(mutex_write);

    return 0;
}