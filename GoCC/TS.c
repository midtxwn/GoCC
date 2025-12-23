

#include "definitions.h"
#include "ABB.h"



TokenPack reservadas[] = 
{
    {PACKAGE,"package"},
    {IMPORT,"import"},
    {FUNC,"func"},
    {CHAN,"chan"},
    {VAR,"var"},
    {FOR,"for"},
    {RANGE,"range"},
    {MAKE,"make"},
    {GO,"go"},
    {BREAK,"break"},
    {CONTINUE,"continue"},
    {FALLTHROUGH,"fallthrough"},
    {FALLTHROUGH,"return"},
};


// Estructura de la tabla de símbolos
typedef struct {
    ABB* arbol;         // ABB para almacenar los lexemas y tokens
} TablaSimbolos;


// Inserta un lexema y su token en la tabla
void TS_insertar(TablaSimbolos* ts, const char* lexema, Token token) {
    ABB_insertar(ts->arbol, (TokenPack){token,(char*)lexema});
}

// Carga palabras reservadas desde un array estático
void _TS_inicializar_keywords(TablaSimbolos* ts) {
    // Array estático de palabras reservadas 
    long unsigned int numTokens = sizeof(reservadas)/sizeof(TokenPack);
    for (long unsigned int i = 0; i < numTokens; i++) {
        TS_insertar(ts, reservadas[i].lexema, reservadas[i].tokenId);
    }
}

// Crea una nueva tabla de símbolos
TablaSimbolos* TS_crear() 
{
    TablaSimbolos* ts = (TablaSimbolos*)malloc(sizeof(TablaSimbolos));
    ts->arbol = ABB_crear();
    _TS_inicializar_keywords(ts);
    return ts;
}

// Destruye la tabla y libera la memoria
void TS_destruir(TablaSimbolos* ts) {
    if (ts) {
        ABB_destruir(ts->arbol);  // Libera todos los lexemas y nodos del ABB
        free(ts);
    }
}

// Busca un lexema. Si no existe, lo inserta automáticamente con token ID
TokenPack TS_buscar(TablaSimbolos* ts, const char* lexema) {
    TokenPack pack = ABB_buscar(ts->arbol, (char*)lexema);

    // Si no se encuentra, insertar con token por defecto (ID)
    if (pack.tokenId == NONE)
        TS_insertar(ts, lexema, ID);
    
    
    return (TokenPack){pack.tokenId, pack.lexema};
}



