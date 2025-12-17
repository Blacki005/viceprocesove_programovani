#include "proj2.h"

//README: Veskery popis funkce jako celku je obsazen v proj2.h.

//hodnota je zvolena pro kompatibilitu na 32 bit platformach, nepredpoklada se vetsi mnozstvi procesu
//cisla procesu jsou rovnomerne rozdelena mezi uredniky a zakazniky
#define PID_MAX_64_BIT 4194304

//makra pro prehlednejsi praci s polozkami pole
#define NZ parameters[0]
#define NU parameters[1]
#define TZ parameters[2]
#define TU parameters[3]
#define F parameters[4]

long *check_arguments(int argc, const char *argv[]) {
    if (argc != 6) {
        return NULL;
    }

    //alokace pole long cisel pro ulozeni argumentu
    long *parameters = calloc(5, sizeof(long int));
    if (parameters == NULL) {
        return NULL;
    }

    //NZ je maximalne LONG_MAX a nemuze byt vyssi nez max_pid / 2
    NZ = strtol(argv[1], NULL, 10);
    if (errno == ERANGE || NZ >= (PID_MAX_64_BIT/2)) {
        free(parameters);
        return NULL;
    }

    //NU je maximalne LONG_MAX a nemuze byt vyssi nez max_pid / 2
    NU = strtol(argv[2], NULL, 10);
    if (errno == ERANGE || NU == 0 || NU >= (PID_MAX_64_BIT/2)) {
        free(parameters);
        return NULL;
    }

    //0 <= TZ <= 10'000 
    TZ = strtol(argv[3], NULL, 10);
    if (TZ < 0 || 10000 < TZ ) {
        free(parameters);
        return NULL;
    }

    //0 <= TU <= 100
    TU = strtol(argv[4], NULL, 10);
    if (TU < 0 || 100 < TU) {
        free(parameters);
        return NULL;
    }

    //0 <= F <= 10'000
    F = strtol(argv[5], NULL, 10);
    if (F < 0 || 10000 < F) {
        free(parameters);
        return NULL;
    }

    return parameters;
}