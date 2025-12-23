# Nombre del ejecutable
TARGET = programa

# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Archivos fuente y objetos
SRCS = main.c asyntax.c alexico.c inputsys.c ABB.c TS.c error.c
OBJS = $(SRCS:.c=.o)

# Regla por defecto
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar archivos .c a .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET)