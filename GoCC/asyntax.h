#ifndef ASYNTAX_H
#define ASYNTAX_H

#include "definitions.h"
#include "alexico.h"
// funcion del analizador sintactico que pedira sin parar a alexico
typedef void ASYNTAX;



ASYNTAX* asyntax_init(ALEXICO* alex);
BOOL asyntax_destruir(ASYNTAX* asyntax);

void parse(ASYNTAX* asyntax);

#endif