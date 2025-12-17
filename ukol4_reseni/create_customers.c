#include "proj2.h"

//README: Veskery popis funkce jako celku je obsazen v proj2.h.

unsigned char create_customers(const unsigned customers_cnt, const unsigned max_wait_time, FILE *output_file) {
    const pid_t main_pid = getpid();
    int ret = 0;

    //vytvoreni NZ procesu zakazniku
    for (unsigned i = 0; i < customers_cnt; i++) {

        //procesy deti nemohou vytvaret dalsi procesy
        if (getpid() == main_pid) {
            ret = fork();
            if (ret < 0) {
                perror_flush("ERROR: Creating customers failed");
                return 1;
            }

            if (IS_CHILD) {

                //vytvoreni jedinecne identity procesu jako lokalni promenne
                struct process_t identity;
                identity.id = i+1;
                identity.type = CUSTOMER;

                sem_wait(mutex_write);
                fprintf_flush(output_file, "%u: Z %u: started\n", ++(*A), identity.id);
                sem_post(mutex_write);

                //po spusteni ceka nahodny cas v intervalu <0, TZ> v milisekundach
                if (max_wait_time != 0) {
                    usleep((rand() % max_wait_time) * 1000);
                }
                
                //pokud je posta otevrena, vyrizuje sluzbu, jinak odchazi domu - kdyz prochazi dvermi posty, posta nemuze zavrit - close_rdy semafor
                sem_wait(close_rdy);
                sem_wait(mutex_write);
                                            
                if (*is_open) {             
                    (*is_waiting) += 1;     
                                            
                    sem_post(mutex_write);

                    enter_for_service(identity, output_file);
                    exit(0);
                } else {
                    //pokud je posta uzavrena, odchazi rovnou domu
                    sem_post(close_rdy);
                    fprintf_flush(output_file, "%u: Z %u: going home\n", ++(*A),identity.id);
                    sem_post(mutex_write);
                    
                    exit(0);
                }
            }
        }
    }

    return 0;

}