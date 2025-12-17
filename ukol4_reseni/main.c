#include "proj2.h"

//globalni sdilene promenne - jejich popis je v proj2.h
int *is_open;
int *is_waiting;
unsigned *A;

//semafory - jejich popis je v proj2.h
sem_t *service1;
sem_t *service2;
sem_t *service3;
sem_t *close_rdy;
sem_t *officer_rdy;
sem_t *choosing_service;
sem_t *taking_break;
sem_t *mutex_write;

int main(int argc, const char *argv[]) {

    long *parameters = check_arguments(argc, argv);
    if (parameters == NULL) {
        goto invalid_arguments;
    }
    //ulozeni parametru programu do promennych a uvolneni pole parameters
    unsigned customers_cnt = parameters[0];     //NZ
    unsigned officers_cnt = parameters[1];      //NU
    unsigned max_wait_time = parameters[2];     //TZ
    unsigned max_break_time = parameters[3];    //TU
    unsigned max_shift_time = parameters[4];    //F
    
    free(parameters);
    
    //otevreni souboru proj2.out v aktualnim adresari
    FILE *output_file = fopen("./proj2.out", "w");
    if (output_file == NULL) {
        goto fopen_failed;
    }


    //vytvoreni sdilene promenne pro stav otevreni posty
    is_open = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0); /// @brief Sdilena promenna indikujici otevreni posty. Pokud je 1, posta je otevrena, pokud je 0, posta je zavrena.
    if (is_open == MAP_FAILED) {
        goto map_failed;
    }
    *is_open = 1;

    //vytvoreni sdilene promenne A
    A = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    if (A == MAP_FAILED) {
        munmap(is_open, sizeof(int));
        goto map_failed;
    }

    // vytvoreni sdilene promennme ukladajici poceet neobslouzenych zakazniku na poste
    is_waiting = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    if (is_waiting == MAP_FAILED) {
        munmap(is_open, sizeof(int));
        munmap(A, sizeof(int));
        goto map_failed;
    }
    *is_waiting = 0;

    //vytvoreni vsech semaforu
    if (!init_semaphores()) {
        goto sem_failed;
    }

    //vytvoreni NZ procesu zakazniku
    if (create_customers(customers_cnt, max_wait_time, output_file)) {
        goto miscarriage;
    }
    
    //vytvoreni NU procesu uredniku
    if (create_officers(officers_cnt, max_break_time, output_file)) {
        goto miscarriage;
    }

    //funkce po zavolani pocka do konce smeny, a pote zavre postu
    close_office(max_shift_time, output_file);

    //cekani na skonceni vsech procesu
    while(wait(NULL) > 0);

    //uvolneni prostredku
    free_resources();
    fclose(output_file);
    munmap(is_open, sizeof(int));
    munmap(A, sizeof(int));
    munmap(is_waiting, sizeof(int));
    return 0;



//navesti resici chybove stavy:
    invalid_arguments:
        errno = EINVAL;
        perror_flush("Error");
        return 1;

    fopen_failed:
        perror_flush("Unable to open the file \"./proj2.out\"");
        return 1;
    
    map_failed:
        perror_flush("Creating shared memory failed");
        fclose(output_file);
        return 1;

    sem_failed:
        perror_flush("Creating semaphore failed");
        fprintf_flush(stderr, "Unlinking all semaphores and ending...\n");
        munmap(is_open, sizeof(int));
        munmap(A, sizeof(int));
        munmap(is_waiting, sizeof(int));
        free_resources();
        fclose(output_file);
        return 1;

    miscarriage:
        //uvolneni vsech vytvorenych procesu zakazniku
        for (unsigned i = 0; i < customers_cnt; i++) {
            sem_post(officer_rdy);
            sem_post(service1);
            sem_post(service2);
            sem_post(service3);
            *is_open = 0;
            *is_waiting = 0;
        }

        //cekani na dobehnuti vsech procesu:
        while(wait(NULL) > 0);

        munmap(is_open, sizeof(int));
        munmap(A, sizeof(int));
        munmap(is_waiting, sizeof(int));
        free_resources();
        fclose(output_file);
        return 1;
}