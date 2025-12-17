#ifndef PRINT_H
#define PRINT_H

#include <stdlib.h>
#include <stdio.h>
#include "stdarg.h"
#include <errno.h>

/**
 * @brief Funkce printf upravena tak, aby po nacteni do bufferu okamzite tiskla a nedochazelo tak k nesrovnalostem kdyz tiskne vice procesu.
 * @param [in] const char *fmt Retezec k tisku, tento argument je stejny jako u funkce printf ze standartni knihovny jazyka C.
 */
void printf_flush(const char *fmt, ...);

/**
 * @brief Funkce fprintf upravena tak, aby po nacteni do bufferu okamzite tiskla a nedochazelo tak k nesrovnalostem kdyz tiskne vice procesu.
 * @param [in] const char *fmt Retezec k tisku, tento argument je stejny jako u funkce fprintf ze standartni knihovny jazyka C.
 */
void fprintf_flush(FILE *file, const char *fmt, ...);

/**
 * @brief Funkce perror upravena tak, aby po nacteni do bufferu okamzite tiskla a nedochazelo tak k nesrovnalostem kdyz tiskne vice procesu.
 * @param [in] const char *s Retezec k tisku nasledovany vysvetlenim momentalniho errna, tento argument je stejny jako u funkce perror ze standartni knihovny jazyka C.
 */
void perror_flush(const char *s);

#endif //PRINT_H