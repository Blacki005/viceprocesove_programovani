#include "proj2.h"

//README: Veskery popis funkce jako celku je obsazen v proj2.h.

unsigned char create_officers(const unsigned officers_cnt, const unsigned max_break_time, FILE *output_file) {
    struct timeval t;
    const pid_t main_pid = getpid();
    int ret = max_break_time;

        //vytvoreni officers_cnt uredniku ve smycce
        for (unsigned i = 0; i < officers_cnt; i++) {

            //do teto sekce vstupuje jen hlavni proces
            if (getpid() == main_pid) {
                ret = fork();
                if (ret < 0) {
                    perror_flush("ERROR: Creating officers failed:");
                    return 1;
                }

                if (IS_CHILD) {
                    //vytvoreni jedinecne identity procesu jako lokalni promenne
                    struct process_t identity;
                    identity.id = i + 1;
                    identity.type = OFFICER;
                    unsigned char served_successfully = 0;
                    
                    sem_wait(mutex_write);//cteni is_waiting a is_open
                    fprintf_flush(output_file, "%u: U %u: started\n", ++(*A),identity.id);

                    while(*is_open || *is_waiting > 0) {//urednik pracuje, dokud je posta otevrena nebo jsou nejaci neobslouzeni zakaznici
                        sem_post(mutex_write);

                        sem_wait(taking_break);
                        sem_wait(mutex_write); //kvuli cteni is_open sdilene promenne

                        if ((*is_open == 1) && (*is_waiting == 0)) {
                            sem_post(mutex_write);

                            //take_break uvolni semafor taking_break uvnitr - neni treba to delat zde
                            take_break(output_file, max_break_time, identity);

                            //pokracuje na zacatek cyklu
                            continue;

                        } else {
                            sem_post(mutex_write);
                            sem_post(taking_break);
                        }

                        //pristup k datum o cekajicich zakaznicich muze mit najednou pouze jeden urednik
                        sem_wait(choosing_service); 

                        //pokud zakaznik v minule iteraci uspesne ukoncil sluzbu zakaznikovi, navysi pocet officer_rdy - uredniku pripravenych ke sluzbe
                        if (!served_successfully) {
                            sem_post(officer_rdy);
                        }
                        
                        //nahodne vybere frontu, kterou bude obsluhovat: cislo 1 az 3
                        gettimeofday(&t, NULL);
                        srand(t.tv_usec * t.tv_sec);
                        identity.service = (rand() % 3) + 1;

                        //pokud je vybrana fronta prazdna, skace se na dalsi iteraci, jinak se obslouzi jeden zakaznik z dane fronty
                        //served successfully se nastavi na 0 pokud uspesne obslouzil zakaznika a na 1, pokud byla fronta prazdna
                        switch (identity.service) {
                            case 1:
                                served_successfully = serve_line(service1, output_file, identity);
                                break;
                            case 2:
                                served_successfully = serve_line(service2, output_file, identity);
                                break; 
                            case 3:
                                served_successfully = serve_line(service3, output_file, identity);
                                break;
                        }

                        sem_wait(mutex_write);
                    }
                    sem_post(mutex_write);


                    sem_wait(mutex_write);
                    fprintf_flush(output_file, "%u: U %u: going home\n", ++(*A),identity.id);
                    sem_post(officer_rdy);
                    sem_post(mutex_write);
                    exit(0);
                }
            }


        }

        return 0;
}