/* TS.h */
#ifndef TS_H
#define TS_H


#include "definitions.h"


// tabla de símbolos abstracta
typedef void TablaSimbolos;

TablaSimbolos* TS_crear();
void TS_destruir(TablaSimbolos* ts);
TokenPack TS_buscar(TablaSimbolos* ts, const char* lexema); // devuelve el token asociado a ese lexema
void TS_insertar(TablaSimbolos* ts, const char* lexema, Token token);

// cargar las palabras reservadas desde array estático
//void TS_inicializar(TablaSimbolos* ts); responsabilidad de ts

#endif