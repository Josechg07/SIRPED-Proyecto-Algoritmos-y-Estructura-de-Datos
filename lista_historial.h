#ifndef LISTA_HISTORIAL_H
#define LISTA_HISTORIAL_H

#include "estructuras.h"

NodoHistorial* crear_nodo_historial(const char* fecha, const char* ubica, const char* est, const char* desc);
void agregar_historial(NodoAVL* nodo_caso, const char* fecha, const char* ubica, const char* est, const char* desc);
void liberar_historial(NodoHistorial* h);

#endif
