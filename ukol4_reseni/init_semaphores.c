#include "proj2.h"

//README: Veskery popis funkce jako celku je obsazen v proj2.h.

int init_semaphores() {
    //popis funkce vsech semaforu je u jejich deklarace v souboru proj2.h
    close_rdy = sem_open("xjanst03 - close_rdy", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (close_rdy == SEM_FAILED) {
        return 0;
    }

    choosing_service = sem_open("xjanst03 - choosing_service", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (choosing_service == SEM_FAILED) {
        return 0;
    }

    taking_break = sem_open("xjanst03 - taking_break", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (taking_break == SEM_FAILED) {
        return 0;
    }

    mutex_write = sem_open("xjanst03 - mutex_write", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (mutex_write == SEM_FAILED) {
        return 0;
    }

    officer_rdy = sem_open("xjanst03 - officer_rdy", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    if (officer_rdy == SEM_FAILED) {
        return 0;
    }

    service1 = sem_open("xjanst03 - sem1", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (service1 == SEM_FAILED) {
        return 0;
    }
    service2 = sem_open("xjanst03 - sem2", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (service2 == SEM_FAILED) {
        return 0;
    }
    service3 = sem_open("xjanst03 - sem3", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    if (service3 == SEM_FAILED) {
        return 0;
    }

    return 1;
}