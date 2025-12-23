/* inputsys.h */
#include "definitions.h"
#ifndef INPUTSYS_H
#define INPUTSYS_H

typedef struct InputSystem InputSystem;

// Interfaz abstracta
InputSystem* input_system_crear(char* srcfile);
void input_system_destruir(InputSystem* sys);
int input_consume_char(InputSystem* sys);
char input_peek_char(InputSystem* sys);
char* buffer_last_n_bytes(InputSystem* sys, int n);
int get_line_number(InputSystem* sys);
#endif