#include "proj2.h"


//README: Veskery popis funkce jako celku je obsazen v proj2.h.

void free_resources() {
    int ret = 0;
        ret = sem_unlink("xjanst03 - sem1");
        if (ret) {
            perror_flush("Semaphore service1 failed to unlink");
        }

        ret = sem_unlink("xjanst03 - sem2");
        if (ret) {
            perror_flush("Semaphore service2 failed to unlink");
        }

        ret = sem_unlink("xjanst03 - sem3");
        if (ret) {
            perror_flush("Semaphore service3 failed to unlink");
        }

        ret = sem_unlink("xjanst03 - close_rdy");
        if (ret) {
            perror_flush("Semaphore close_rdy failed to unlink");
        }


        ret = sem_unlink("xjanst03 - choosing_service");
        if (ret) {
            perror_flush("Semaphore choosing_service failed to unlink");
        }

        ret = sem_unlink("xjanst03 - taking_break");
        if (ret) {
            perror_flush("Semaphore taking_break failed to unlink");
        }

        ret = sem_unlink("xjanst03 - mutex_write");
        if (ret) {
            perror_flush("Semaphore mutex_write failed to unlink");
        }

        ret = sem_unlink("xjanst03 - officer_rdy");
        if (ret) {
            perror_flush("Semaphore officer_rdy failed to unlink");
        }

}