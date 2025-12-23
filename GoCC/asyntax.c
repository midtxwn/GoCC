#include "definitions.h"
#include "alexico.h"

// el uso de la estructura de datos debe ser abstracto

typedef struct 
{
    ALEXICO* alexico;
} ASYNTAX;
ASYNTAX* asyntax_init(ALEXICO* alex)
{
    ASYNTAX* asyntax = (ASYNTAX*) malloc(sizeof(ASYNTAX));
    asyntax->alexico = alex;
    return asyntax;
}

BOOL asyntax_destruir(ASYNTAX* asyntax)
{
    free(asyntax);
    return TRUE;
}

void parse(ASYNTAX* asyntax)
{
    TokenPack pack; // pedimos tokenpack a analizador lexico (token, lexema)
    do
    {
        pack = next_token(asyntax->alexico);
        printf("<\t%d\t,\t%s\t>\n", pack.tokenId, pack.lexema); // imprimimos
        if (
            pack.tokenId == INT_LITERAL ||
            pack.tokenId == FLOAT_LITERAL ||
            pack.tokenId == IMAGINARY_LITERAL ||
            pack.tokenId == STRING_LITERAL
        )
                free(pack.lexema); // es responsabilidad del analizador sintactico liberar los numeric y string literals. Los ids son destruidos al 
                //destruir la tabla de simbolos. las palabras clave y operadores se almacenan en la seccion read only (.rdonly) del programa, por lo que no es necesario liberarlo.

    } while (pack.tokenId != EOF);
    

}

