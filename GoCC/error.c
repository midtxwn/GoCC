#include "error.h"


void print_charp(const char*toprint)
{
    fprintf(stderr, "%s", toprint);
}

void print_error(const char *msg, int mode, int line) 
{
    if (mode == ERROR_LOG || mode == ERROR_PERROR) 
    {
        fprintf(stderr, "Error: \"%s\" at Line: %d\n", msg, line);
    }
    
    if (mode == ERROR_PERROR) 
    {
        perror("System error");
    }


}
