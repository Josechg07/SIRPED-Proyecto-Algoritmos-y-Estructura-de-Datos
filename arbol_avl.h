#ifndef ARBOL_AVL_H
#define ARBOL_AVL_H

#include "estructuras.h"

/*
 * Inserta un caso nuevo en el árbol, indexado por id_caso.
 * Si el id ya existe, esta función NO debe usarse para actualizarlo:
 * usa agregar_historial() directamente sobre el nodo (ver main.c,
 * opción "Actualizar Estado"). insertar_caso() asume que el id es nuevo.
 * Retorna la nueva raíz del árbol (puede cambiar por rotaciones).
 */
NodoAVL* insertar_caso(NodoAVL* nodo, const char* id, const char* nombre,
                        const char* fecha, const char* zona,
                        const char* est, const char* desc);

#endif
