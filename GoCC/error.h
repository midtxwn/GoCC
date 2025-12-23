#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

// error flags
#define ERROR_LOG     1  // Just log the error message
#define ERROR_PERROR  2  // Log the error message and call perror()
// Function to print an error message
void print_error(const char *msg, int mode, int lineNumber);
void print_charp(const char*toprint);
#endif // ERROR_H
