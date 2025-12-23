#ifndef DEFINITIONS
#define DEFINITIONS


#include <stdio.h>
#include <stdlib.h>

#define BOOL int
typedef int Token;

typedef struct 
{
    Token tokenId;
    char* lexema;
} TokenPack;


#define FALSE 0
#define TRUE 1

#define NONE                -1
#define INVALID_TOKENID     0
#define PACKAGE             300       // "package"
#define ID                  301            // "main", "sum", "s", "c", "v", "i", "j", etc.
#define IMPORT              302        // "import"
#define STRING_LITERAL      303 // "fmt", "Partial results...", etc.
#define FUNC                304          // "func"
#define CHAN                306          // "chan"
#define VAR                 307           // "var"
#define FOR                 308           // "for"
#define RANGE               309         // "range"
#define CHAN_SEND           310     // "<-" (envío a canal)
#define MAKE                311          // "make"
#define GO                  312            // "go"
#define DECLARE_ASSIGN      313 // ":="
#define ADD_ASSIGN          314     // "+=" (operador de asignación de suma)
#define INT_LITERAL         315    // "0", "2", "-9"
#define FLOAT_LITERAL       316  // "3.14", "1.e0", "4.e+0", "0xBadFace42*1e-11", ".1273E2" "4.e+0", "0xBadFace42*1e-11", ".1273E2"
#define IMAGINARY_LITERAL   317
#define MINUSMINUS          318
#define MINUSEQUALS         319
#define PLUSPLUS            320
#define PLUSEQUALS          321
#define BREAK               322
#define CONTINUE            323
#define FALLTHROUGH         324
#define RETURN              325
#define GREATEREQUALS       326
#define LESSEREQUALS        327
#define EQUALSEQUALS        328


#endif
