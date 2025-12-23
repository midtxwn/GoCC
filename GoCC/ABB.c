#include "ABB.h"
#include <stdlib.h>
#include <string.h>


typedef struct NodoABB {
    TokenPack dato;
    struct NodoABB *izq;
    struct NodoABB *der;
} NodoABB;

struct ABB {
    NodoABB* raiz;
};

void _destruir_nodos(NodoABB *nodo) 
{
    if (nodo == NULL) return;
    _destruir_nodos(nodo->izq);
    _destruir_nodos(nodo->der);
    if(nodo->dato.tokenId == ID) // si es un id, liberar. En otro caso, es responsabilidad de un tercero liberarlo
        free(nodo->dato.lexema);

    free(nodo);
}

ABB* ABB_crear(void) 
{
    ABB* arbol = (ABB*)malloc(sizeof(ABB));
    if (arbol) arbol->raiz = NULL;
    return arbol;
}

void ABB_insertar(ABB *arbol, TokenPack dato)
{
    NodoABB **actual = &arbol->raiz;
    while (*actual)
    {
        int cmp = strcmp(dato.lexema, (*actual)->dato.lexema); // comparacion del lexema
        actual = (cmp < 0) ? &(*actual)->izq : &(*actual)->der;
    }
    *actual = (NodoABB*)malloc(sizeof(NodoABB));
    (*actual)->dato = dato;
    (*actual)->izq = (*actual)->der = NULL;
}

TokenPack ABB_buscar(ABB *arbol, const char *lexema)
{
    NodoABB *actual = arbol->raiz;
    while (actual)
    {
        int cmp = strcmp(lexema, actual->dato.lexema); // comparar por lexema
        if (cmp == 0)
            return actual->dato;
        actual = (cmp < 0) ? actual->izq : actual->der;
    }
  
 
    return (TokenPack){NONE,NULL};
}

void ABB_destruir(ABB *arbol) {
    if (!arbol) return;
    _destruir_nodos(arbol->raiz); // se liberaran unicamente los lexemas que pertenezcan a token ID
    free(arbol);
}
