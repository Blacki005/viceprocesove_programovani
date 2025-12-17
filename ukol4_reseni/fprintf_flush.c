#include "print.h"

//printf s flush bufferu - tiskne se okamzite
void fprintf_flush(FILE *file, const char *fmt, ...) {
    va_list argv;
    va_start(argv, *fmt);
    vfprintf(file, fmt, argv);
    fflush(file);
    va_end(argv);
    return;
}