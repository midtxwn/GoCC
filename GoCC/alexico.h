#ifndef ALEXICO_H
#define ALEXICO_H

#include "definitions.h"
#include "inputsys.h"
#include "TS.h"

typedef void ALEXICO;
// inicializa el analizador léxico
ALEXICO* alexico_init(InputSystem* input, TablaSimbolos* ts);

void alexico_destruir(ALEXICO* alexico);

// Obtiene el siguiente token (función exportada para el sintáctico)
TokenPack next_token(ALEXICO* alexico);

#endif