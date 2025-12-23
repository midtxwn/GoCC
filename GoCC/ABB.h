
#ifndef ABB_H
#define ABB_H

#include "definitions.h"


typedef struct ABB ABB;

// crear ABB
ABB* ABB_crear(void);

// insertar tokenpack en ABB
void ABB_insertar(ABB* arbol, TokenPack dato);

// buscar tokenpack por lexema
TokenPack ABB_buscar(ABB* arbol, const char* lexema);

// destruir abb y liberar memoria
void ABB_destruir(ABB* arbol);

#endif // ABB_H