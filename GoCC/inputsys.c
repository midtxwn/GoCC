/* inputsys.h */
#include "definitions.h"
#include <stdlib.h>
#include "error.h"

#define BUFFER_SIZE 40
#define BUFFER_EFFECTIVE_SIZE (BUFFER_SIZE - 1)
#define CUSTOM_EOF EOF




typedef struct {
    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    char bufferIndex;
    char* currPointer;
    int lineNumber;
    FILE* file;

} InputSystem;

InputSystem* input_system_crear(char* srcfile)
{

    InputSystem* is = (InputSystem*) malloc(sizeof(InputSystem)); //reservamos la estructura interna
    is->buffer1[BUFFER_EFFECTIVE_SIZE] = CUSTOM_EOF; // podemos usar custom eof si queremos usar un eof distinto al propio del sistema. En este caso es el EOF default
    is->buffer2[BUFFER_EFFECTIVE_SIZE] = CUSTOM_EOF; // ponemos EOF al final de los bufferes
    is->currPointer = is->buffer1; // puntero actual apunta al primer buffer
    is->file = fopen(srcfile,"r"); //abrimos el fichero en modo lectura
    is->bufferIndex = 0; // contador que se usa para determinar si el buffer es el 1 o el 2
    int read = fread(is->buffer1,sizeof(char),BUFFER_EFFECTIVE_SIZE,is->file); // cargamos el primer buffer, con el tamanho de buffer efectivo (sin incluir los EOF puestos manualmente)
    
    
    if (read < BUFFER_EFFECTIVE_SIZE)
    {
        is->currPointer[read] = EOF; // si el fichero es tan pequenho que no completa ni un buffer, hay que poner EOF
    }

    return is;

}

int get_line_number(InputSystem* sys)
{
    return sys->lineNumber; // "getter" de contador de linea, utilizado para logear errores
}


void input_system_destruir(InputSystem* sys)
{
    if(!sys){return;} 
    if (sys->file)
    {
        fclose(sys->file); //liberar FILE* del fichero
    }
    free(sys); //liberar estructura 
}
BOOL _loadblock(InputSystem* sys) // carga bloque en el puntero actual
{
    if(sys->buffer1 != sys->currPointer && sys->buffer2 != sys->currPointer)
        return FALSE; //bad call, puntero debe estar al inicio de un bloque

    int read = fread(sys->currPointer,sizeof(char),BUFFER_EFFECTIVE_SIZE,sys->file); // load buffer
    if (read < BUFFER_EFFECTIVE_SIZE)
    {
        sys->currPointer[read] = EOF; // si hemos llegado al final del fichero, fread retornara read menor que los esperados, asi que ponemos EOF
    }
    return TRUE;
}

char _increment_pointer(InputSystem* sys) // funcion que controla incrementar el puntero
{
    char consumedChar = *(sys->currPointer);
    if (consumedChar == EOF) // Si se lee EOF antes de incrementar, es porque el fichero se ha terminado, porque al incrementar puntero se comprueba que, si es EOF, se alterna el buffer
        return EOF;
    sys->currPointer++;
    if(*(sys->currPointer) == CUSTOM_EOF) // fin de buffer
    {
        sys->currPointer = ((sys->bufferIndex & 0x1) ? sys->buffer1 : sys->buffer2); // numero par, buffer1, numero impar, buffer2
        sys->bufferIndex++; //incrementamos "contador" (solo es relevante el bit menos significativo)
        if (_loadblock(sys) == FALSE)
        {
            print_error("Input System: Error cargando bloque: Puntero distinto de incio de buffers.",ERROR_LOG,get_line_number(sys));
            sys->currPointer = ((sys->bufferIndex & 0x1) ? sys->buffer1 : sys->buffer2); // intento de recuperacion
            return 0;
        } 
    
    }
    
    return consumedChar;

}


char _decrement_pointer(InputSystem* sys) // funcion para decrementar puntero usada en buffer_last_bytes
{
    char consumedChar = *(sys->currPointer);

    if(sys->currPointer == sys->buffer1) // poner puntero al final del otro buffer
        sys->currPointer = sys->buffer2 + BUFFER_EFFECTIVE_SIZE - 1;
    else if (sys->currPointer == sys->buffer2)
        sys->currPointer = sys->buffer1 + BUFFER_EFFECTIVE_SIZE - 1;
    else
        sys->currPointer--; // si no estabamos al principio de un buffer, simplemente decrementar
    
    return consumedChar;

}




int input_consume_char(InputSystem* sys)
{
    char consumed = _increment_pointer(sys);
    if (consumed == '\n') // chequeamos si es un salto de linea para incrementar contador de linea
        sys->lineNumber++; // se podria hacer en alexico (que tambien debe chequear salto de linea) pero ello implicaria mezclar responsabilidades
    return consumed;
}

char input_peek_char(InputSystem* sys) // devuelve el char al que apunte actualmente el puntero SIN CONSUMIRLO
{                                      //asi que, de forma efectiva, permite "ver" por adelantado el buffer, utilizado para implementar lookahead
    return *(sys->currPointer);
}

//copia los ultimos n bytes, en caso de que sea posible, de los bufferes internos del sistema de entrada
// y devuelve un puntero al buffer donde se copiaron los datos. La llamada no garantiza copiar la totalidad
// de los bytes. En caso de que no pueda, lanzara un error del sistema de entrada
//NOTA: NO incluye el caracter actual al que apunta el buffer interno, es decir, 
// copia los ultimos n caracteres consumidos.
char* buffer_last_n_bytes(InputSystem* sys, int n)
{
    
    int nchars = (n < (BUFFER_EFFECTIVE_SIZE)) ? (n) : (BUFFER_EFFECTIVE_SIZE); // numero de caracteres a copiar = min(pedidos por el usuario, maximo interno del sistema)


    char* savedPtr = sys->currPointer; // salvamos el valor actual del puntero que restauraremos despues
    char* buff = (char*) malloc(nchars + 1); // reservamos para nchars

    if(!buff) // malloc error
    {
        print_error("Input system: Malloc call failed",ERROR_LOG,get_line_number(sys));
        return NULL;
    }

    _decrement_pointer(sys); //decrementamos puntero para no incluir el caracter actual no consumido
    for(int i = nchars - 1; i >= 0; --i) // copiamos desde fin hacia atras
    {
        buff[i] = _decrement_pointer(sys);
    }
    buff[nchars] = '\0'; 

    if(n > (BUFFER_EFFECTIVE_SIZE)) // si la copia excede el numero de bytes maximos, se lanza error, y el llamante debe saber que el buffer estara cortado cogiendo los n ultimos bytes del string, siendo n el tamanho maximo de lexema
    {
        print_charp(buff); // imprimimos el buffer recortado
        print_error("Input system: INPUT TOO LONG",ERROR_LOG,get_line_number(sys));
        
    }
    sys->currPointer = savedPtr; // restauramos el puntero
    return buff;

}