#include "inputsys.h"
#include "TS.h"
#include "alexico.h"
#include "asyntax.h"

int main(int argc, char *argv[]) {
    // Inicialización
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    InputSystem* input = input_system_crear(argv[1]); // Usa el archivo de la línea de comandos

    //InputSystem* input = input_system_crear("./concurrentSum.go"); // creamos input system
    TablaSimbolos* ts = TS_crear(); // inicializamos tabla de simbolos, requerida por analizador lexico
    ALEXICO* analizadorLexico = alexico_init(input, ts); // inicializamos analizador lexico, se comunica unidireccionalmente con sistema de entrada y tabla de simbolos
    ASYNTAX* analizadorSintactico = asyntax_init(analizadorLexico); // inicializamos analizador sintactico, se comunica unidireccionalemnte con analizador lexico

    
    
    // ejecución del asyntax (parsear el codigo, funcion principal que pide componentes lexicos a alexico)
    parse(analizadorSintactico);
    
    // Liberación
    TS_destruir(ts);
    alexico_destruir(analizadorLexico);
    asyntax_destruir(analizadorSintactico);
    input_system_destruir(input);
    
    return 0;
}