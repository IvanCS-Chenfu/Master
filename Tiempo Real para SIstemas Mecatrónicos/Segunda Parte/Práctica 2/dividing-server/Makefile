#-----------------------------------------------------------------------
# File: Makefile
#
# $ make all        // compiles every C_source_file into diferent execs
# $ make                                // same as $make all
# $ make <C_source_file_w/o_extension>  // compiles 1 program
#
# Author: Sergio Romero Montiel
#
# Created on April 20th, 2016
#-----------------------------------------------------------------------

#-----------------------FILES-------------------------------------------
# Lista de fuentes
SOURCES := ${wildcard *.c}
# Ejecutables
EXECS := ${SOURCES:.c=}

#-----------------------TOOLS-------------------------------------------
# Compiladores y Enlazadores (no modificar, usamos los por defecto)
#CC =
# Opciones para el compilador
CFLAGS = -Wall
# Opciones de enlazado
LDFLAGS = -pthread

# ----------------------RULES-------------------------------------------
# Targets y sufijos
.PHONY: all clean
# regla para obtener todos los ejecutables
all: $(EXECS)
# regla para rehacer el ejecutable
clean:
	-rm -fv $(EXECS)
#-----------------------------------------------------------------------
