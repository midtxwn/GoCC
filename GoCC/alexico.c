#include "definitions.h"
#include "inputsys.h"
#include "TS.h"
#include <ctype.h>
#include "error.h"

typedef struct {
    //placeholder
    InputSystem* is;
    TablaSimbolos* ts;
    Token LastToken;
    
} ALEXICO;

//inicializar analizador lexico
ALEXICO* alexico_init(InputSystem* input, TablaSimbolos* ts)
{
    ALEXICO* alexico = (ALEXICO*) malloc(sizeof(ALEXICO)); // inicializamos estructura
    alexico->is = input; // sistema de entrada asociado
    alexico->ts = ts; // tabla de simbolos asociada
    alexico->LastToken = 0; // se utilizara para insertar semicolons
    return alexico;
}

void alexico_destruir(ALEXICO* alexico)
{
    if(alexico) //destruir estructura
        free(alexico);
}

BOOL _is_non_token(char c) //si es un caracter que delimita un token
{
    return (c == ' ' || c == '\n' || c == '\t');
}


//funcion que se encarga de saltar espacios, tabs, etc, y de detectar si es necesario insertar un semicolon
TokenPack _skip_spaces_and_detect_endline(ALEXICO* alexico) 
{ 
    BOOL has_newline = FALSE;
    BOOL in_comment = FALSE;
    int c;
    while (TRUE) {
        c = input_peek_char(alexico->is);
            
        if (!_is_non_token(c) && (c != '/') && !in_comment) break; // si es un caracter que potencialmente pertenece a un token, salir y dejar que los automatas lo procesen

        input_consume_char(alexico->is);
        
        // si hemos encontrado newline
        if (c == '\n') {
            
            has_newline = TRUE;
            in_comment = FALSE;
        }
        
        // manejar comentarios
        if (c == '/' && !in_comment) { // la barra puede ser un comentario o la division

            int next = input_peek_char(alexico->is);
            if (next == '/') {
                // comentario de linea, actua como endline
                has_newline |= TRUE;
                in_comment = TRUE;
            }
            else if (next == '*') {
                // comentario de bloque, hay que buscar newlines
                BOOL comment_has_newline = FALSE;
                input_consume_char(alexico->is); // consumir '*'
                while (TRUE) {
                    int comment_char = input_peek_char(alexico->is); 
                    if (comment_char == EOF) break; 
                    
                    if (comment_char == '\n') comment_has_newline = TRUE; // habra que devolver semicolon
                    
                    input_consume_char(alexico->is); // consumimos caracter del comentario
                    if (comment_char == '*' && input_peek_char(alexico->is) == '/') { // comprobar si hemos alcanzado el final del comentario
                        input_consume_char(alexico->is);
                        in_comment = FALSE;
                        break;
                    }
                }
                has_newline |= comment_has_newline;
                //in_comment = FALSE;
            }
            else
            {
                return (TokenPack) {'/',"/"}; // si no estaba seguido de otra barra o un asterisco, era un operador de divison
            }
        }
    }


    if (has_newline || c == EOF) { 
        // check si ultimo token requiere ;
        Token last_type = alexico->LastToken;
        

        BOOL needs_semicolon = (
            last_type == ID ||
            last_type == INT_LITERAL ||
            last_type == FLOAT_LITERAL ||
            last_type == IMAGINARY_LITERAL ||
            last_type == STRING_LITERAL ||
            last_type == BREAK ||
            last_type == CONTINUE ||
            last_type == FALLTHROUGH ||
            last_type == RETURN ||
            last_type == PLUSPLUS ||
            last_type == MINUSMINUS ||
            last_type == ')' ||
            last_type == ']' ||
            last_type == '}'
        );

        if (needs_semicolon) 
        {
            return (TokenPack){';',";"};
        }
    }

    if(c == EOF)
        return (TokenPack){EOF, NULL};

    
    return (TokenPack){NONE, NULL};
}



TokenPack _process_string_literal(ALEXICO* alexico)
{
    input_consume_char(alexico->is);  // consumir open quota
    int lexLenght = 1;
    char peekchar;
    // escanear hasta closing quote
    while ((peekchar = input_peek_char(alexico->is)) != '"') {
        if (peekchar == '\\')
        {
            input_consume_char(alexico->is); // hay un escapado. Consumimos 2 caracteres
            input_consume_char(alexico->is);
            lexLenght += 2; 
            continue;
        }
        {
            /* code */
        }
        
        if (peekchar == EOF) {
            return (TokenPack){INVALID_TOKENID, "UNTERMINATED_STRING"};
        }

        
        input_consume_char(alexico->is);
        lexLenght++;
    }
    input_consume_char(alexico->is);  // consumir closing quota 
    lexLenght++;
    // reservar buffer para string literal
 
    

    // copiar del buffer de sistema
  
    char* content = buffer_last_n_bytes(alexico->is, lexLenght); // nota, al ser un string literal, si el input system no es capaz de copiarlo (p.ej, porque excede el tamanho maximo de lexema
                                                                // entonces se podria producir un error en cascada porque el string queda mal formado (falta la primera '"'), asi que el analizador sintactico podria deducir informacion en caso de que se encuentre asi el string literal, pero es una cuestion de disenho que no implementamos (el analizador sintactico)
    if (!content) {
        return (TokenPack){INVALID_TOKENID, "MEMORY_ERROR"}; 
    }

    
    return (TokenPack){STRING_LITERAL,content};
}


TokenPack _process_alphanumeric_lexeme(ALEXICO* alexico)
{
    int lexLength = 0;  

    
    char peekchar = input_peek_char(alexico->is);
    while(  isalnum(peekchar) || peekchar == '_') // mientras sea caracter alfanumerico, consumimos
    {
        input_consume_char(alexico->is);
        peekchar = input_peek_char(alexico->is);
        lexLength++;
    }
    char* copiedLexeme = NULL;
    if(lexLength > 0)
        copiedLexeme = buffer_last_n_bytes(alexico->is,lexLength); //copiamos el lexema

    if(!copiedLexeme)
    {

        print_error("Alexico: Lexema vacio/error en malloc",ERROR_LOG,get_line_number(alexico->is));
        return (TokenPack) {ID, NULL};
    }

    TokenPack foundTokenPack = TS_buscar(alexico->ts, copiedLexeme);
    if (foundTokenPack.tokenId == NONE) // es un ID, TS buscar lo habra insertado automaticamente
    {
        return (TokenPack){ID,copiedLexeme};
    }

    if(foundTokenPack.tokenId == INVALID_TOKENID)
    {
        print_error("Alexico: Token invalido encontrado en TS",ERROR_LOG,get_line_number(alexico->is));
    }
    free(copiedLexeme);
    return(TokenPack){foundTokenPack.tokenId,foundTokenPack.lexema};

}

// funcion auxiliar que devuelve el numero de digitos detectados y los consume del input
int _process_decimal_digits(ALEXICO* alexico) 
{
    int lexLength = 0;
    char peekChar = input_peek_char(alexico->is);
    BOOL isdigit_next_char = isdigit(peekChar);

    if(!isdigit_next_char)
    {
        return 0;
    }

    while(isdigit_next_char || peekChar == '_')
                {
                    lexLength++;
                    input_consume_char(alexico->is);
                    char thisChar = peekChar; // guardamos valor de peekchar antes de actualizarlo
                    isdigit_next_char = isdigit(peekChar = input_peek_char(alexico->is));
                    if (thisChar == '_' && !isdigit_next_char) // si hay '_' consecutivos, error
                    {
                        print_error("Alexico: Literal numerico malformado: Dos '_' consecutivas",ERROR_LOG,get_line_number(alexico->is));
                        return -1;
                    }

                }
    return lexLength;
}

//funcion encargada de procesar un lexema perteneciente a un token numerico
TokenPack _process_numeric_literal(ALEXICO* alexico, char initialChar) 
{
    // initialChar> rule exception para float que empieza por '.',  p.ej, .1238E
    char peekChar = input_peek_char(alexico->is);
    int lexLength = 0;
    BOOL validInitial = 0; // para ciertas reglas es necesario como minimo un digito, se utiliza esta variable para almacenar dicha condicion
    TokenPack returnToken = {INVALID_TOKENID,NULL};

 
    if(peekChar == '0') //si empieza por 0, puede ser hex
    {
        validInitial = 1; // inicial valida
        lexLength++;
        input_consume_char(alexico->is);
        peekChar = input_peek_char(alexico->is);
        if(tolower(peekChar) == 'x') // comprobar si es hex
        {
                
                lexLength++;
                input_consume_char(alexico->is);

                peekChar = input_peek_char(alexico->is);
                if(peekChar == '_') // saltar _ por si empieza por 0x_
                {
                    lexLength++;
                    input_consume_char(alexico->is);
                    peekChar = input_peek_char(alexico->is);
                }
                
                if(!isxdigit(peekChar)) // garantizar al menos un digito despues de 0x o 0x_
                {
                    returnToken.tokenId = INVALID_TOKENID;
                    print_error("Alexico: Literal numerico malformado: se esperaba un digito",ERROR_LOG,get_line_number(alexico->is));
                }
                
                BOOL isx_next_char = isxdigit(peekChar = input_peek_char(alexico->is));

                while(isx_next_char || peekChar == '_') // mientras sea digito hexadecimal
                {
                    lexLength++;
                    input_consume_char(alexico->is);
                    char thisChar = peekChar; // guardamos valor de peekchar antes de actualizarlo
                    isx_next_char = isxdigit(peekChar = input_peek_char(alexico->is));
                    if (thisChar == '_' && !isx_next_char) // si hay _ consecutivos, error
                    {
                        print_error("Alexico: Literal hexadecimal malformado: '_' consecutivos",ERROR_LOG,get_line_number(alexico->is));
                        returnToken.tokenId = INVALID_TOKENID;
                    }

                }
                

        }
        returnToken.tokenId = INT_LITERAL;

         
    }


    if (((lexLength <= 1) && isdigit(peekChar))) // en caso de que no fuese hex, simplemente empezaba por 0 o en caso de que no empezase por 0
    { 
          returnToken.tokenId = INT_LITERAL;
        //puede ser decimal, puede ser imaginary , puede ser float
        if (!initialChar) // si no empieza por ., hay que consumir decimales y sigue siendo int literal
        {
            validInitial = 1;
            lexLength += _process_decimal_digits(alexico);
            peekChar = input_peek_char(alexico->is);
        }

        if(peekChar == '.' || (initialChar && initialChar == '.')) // float literal
        {
            validInitial = 1; // valid initial porque si esta llamada se realizó, es porque el llamante se aseguró de que después del punto hubiese un dígito
            returnToken.tokenId = FLOAT_LITERAL;
            lexLength++;
            if (!initialChar) // si el punto viene dado por el llamante, es que ya se consumio (el caso en el que el numero empieza por .)
            {
                input_consume_char(alexico->is);
            }
            
            
            lexLength += _process_decimal_digits(alexico);
            peekChar = input_peek_char(alexico->is);
            
        }

        if(tolower(peekChar) == 'e' && validInitial)  // comprobar exponente
        {
            lexLength++;
            input_consume_char(alexico->is);
            peekChar = input_peek_char(alexico->is);
            if (peekChar == '+' || peekChar == '-')
            {
                lexLength++;
                input_consume_char(alexico->is);
            }
            //asegurarse de al menos 1 digito decimal
            if( !(isdigit(input_peek_char(alexico->is))) )
            {
                print_error("Alexico: Literal flotante malformado: se esperaba un digito despues de 'e' o '+' o '-'",ERROR_LOG,get_line_number(alexico->is));
                returnToken.tokenId = INVALID_TOKENID;
                return returnToken;
            }

            lexLength += _process_decimal_digits(alexico);
            peekChar = input_peek_char(alexico->is);

        }
    }
    if(validInitial && input_peek_char(alexico->is) == 'i') // comprobar imaginario
    {
        returnToken.tokenId = IMAGINARY_LITERAL;
        input_consume_char(alexico->is);
        lexLength++;
    }

    if(returnToken.tokenId != INVALID_TOKENID)
    {
        returnToken.lexema = buffer_last_n_bytes(alexico->is,lexLength); //copiar el lexema
        if(!returnToken.lexema)
        {
            print_error("Alexico: Lexema vacio/error en malloc",ERROR_LOG,get_line_number(alexico->is));
            return returnToken;
        }
    }
  return returnToken;

}


//funcion principal que es llamada desde el analizador sintactico. Devuelve un token pack (token, lexema)
TokenPack next_token(ALEXICO* alexico)
{
    
    TokenPack skipToken = _skip_spaces_and_detect_endline(alexico);
    if (skipToken.tokenId != NONE) // lookahead no permite mirar 2 caracteres sin consumir uno. Para detectar que es una division y no un comentario, skip spaces 
    //  tiene que consumir una de las dos barras. Si era una division, devolvera el caracter de division en el tokenpack.
    { // Por tanto, el caracter / es uno especial que no es comprobado en el switch de abajo.
        alexico->LastToken = skipToken.tokenId;
        return skipToken; // Puede ser '/' o ';'
    }

    TokenPack returnPack;
    char firstChar = input_peek_char(alexico->is);

    //toma de decision en funcion del primer caracter

    if (isalpha(firstChar) || firstChar == '_')
    {

        returnPack = _process_alphanumeric_lexeme(alexico); //es una cadena alfanumerica
    }

    else if(isdigit(firstChar))
    {
        returnPack = _process_numeric_literal(alexico,0); // es un literal numerico
    }
    else{ // es operador, eof, etc
    char peekedChar;
    switch (firstChar)
    {
        case EOF:

        returnPack.tokenId = EOF;
        returnPack.lexema = "EOF";
        break;
        case '=': //lookahead, depende del siguiente char
            input_consume_char(alexico->is);
            if ( input_peek_char(alexico->is) == '=')
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = EQUALSEQUALS;
                returnPack.lexema = "==";
            }
            else
            {
                returnPack.tokenId = '=';
                returnPack.lexema = "=";
            }

        break;

        case '-': //lookahead
            input_consume_char(alexico->is);
            peekedChar = input_peek_char(alexico->is);
            if ( peekedChar == '-' )
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = MINUSMINUS;
                returnPack.lexema = "--";
                break;
            }
            else if(peekedChar == '=')
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = MINUSEQUALS;
                returnPack.lexema = "-=";
                break;
            }
            else
            {
                returnPack.tokenId = '-';
                returnPack.lexema = "-";
                break;
            }

        break;

        case '+':
            input_consume_char(alexico->is);
            peekedChar = input_peek_char(alexico->is);
            if ( peekedChar == '+' )
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = PLUSPLUS;
                returnPack.lexema = "++";
                break;
            }
            else if(peekedChar == '=')
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = PLUSEQUALS;
                returnPack.lexema = "+=";
                break;
            }
            else
            {
                returnPack.tokenId = '+';
                returnPack.lexema = "+";
                break;
            }

        break;


        case '"': // si empieza por '"', es un string literal
            returnPack = _process_string_literal(alexico);

        break;

        case '*':
        input_consume_char(alexico->is);
        returnPack.tokenId = '*';
        returnPack.lexema = "*";
        break;

        case '(':
        input_consume_char(alexico->is);
        returnPack.tokenId = '(';
        returnPack.lexema = "(";
        break;

        case ')':
        input_consume_char(alexico->is);
        returnPack.tokenId = ')';
        returnPack.lexema = ")";

        break;

        case '{':
        input_consume_char(alexico->is);
        returnPack.tokenId = '{';
        returnPack.lexema = "{";

        break;
        
        case '}':
        input_consume_char(alexico->is);
        returnPack.tokenId = '}';
        returnPack.lexema = "}";
        break;

        case '[':
        input_consume_char(alexico->is);
        returnPack.tokenId = '[';
        returnPack.lexema = "[";
        break;

        case ']':
        input_consume_char(alexico->is);
        returnPack.tokenId = ']';
        returnPack.lexema = "]";
        break;
            /*
        case '/':
        input_consume_char(alexico->is);
        returnPack.tokenId = '/';
        returnPack.lexema = "/";
        break;
    */
        case ',':
        input_consume_char(alexico->is);
        returnPack.tokenId = ',';
        returnPack.lexema = ",";
        break;

        case ':':
        input_consume_char(alexico->is);
        if(input_peek_char(alexico->is) == '=')
        {
            input_consume_char(alexico->is);
            returnPack.tokenId = DECLARE_ASSIGN;
            returnPack.lexema = ":=";
        }
        else
        {
            returnPack.tokenId = ':';
            returnPack.lexema = ":";
        }
      


        break;

        case ';':
        input_consume_char(alexico->is);
        returnPack.tokenId = ';';
        returnPack.lexema = ";";
      
        break;

        case '>':
        input_consume_char(alexico->is);
        if(input_peek_char(alexico->is) == '=')
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = GREATEREQUALS;
                returnPack.lexema = ">=";
            }
        else 
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = '>';
                returnPack.lexema = ">";
            }
        break;

        
        case '<':
        input_consume_char(alexico->is);
        if(input_peek_char(alexico->is) == '=')
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = LESSEREQUALS;
                returnPack.lexema = "<=";
            }
        else if(input_peek_char(alexico->is) == '-')
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = CHAN_SEND;
                returnPack.lexema = "<-";
            }
        else 
            {
                input_consume_char(alexico->is);
                returnPack.tokenId = '<';
                returnPack.lexema = "<";
            }

        break;

        case '.':

        input_consume_char(alexico->is);
        if(isdigit(input_peek_char(alexico->is))) // aqui se comprueba el caso de que el numeric literal empiece por .
        {
            returnPack = _process_numeric_literal(alexico, '.'); // el segundo parametro informa a la funcion de que empieza por punto
        }
        else
        {
        returnPack.tokenId = '.';
        returnPack.lexema = ".";
        }

        
        break;

        


        default:
        returnPack.tokenId = INVALID_TOKENID;
        returnPack.lexema = NULL;
            break;
        }
    }

    if (returnPack.tokenId == INVALID_TOKENID)
    {
        // error handle
        print_error("Alexico: Lexema perteneciente a token desconocido",ERROR_LOG,get_line_number(alexico->is));
        // error handle
    }
    alexico->LastToken = returnPack.tokenId; // guardamos el ultimo token para decidir si se inserta semicolon
    return returnPack;


        
        


}

