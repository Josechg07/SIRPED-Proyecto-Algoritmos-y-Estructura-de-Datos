#ifndef CONSULTAS_H
#define CONSULTAS_H

#include "estructuras.h"

/* Búsqueda exacta por id_caso. O(log n): el árbol está indexado por id. */
NodoAVL* buscar_por_id(NodoAVL* raiz, const char* id);

/* Imprime un caso y todo su historial cronológico de estados. */
void mostrar_caso(NodoAVL* nodo);

/*
 * Consultas secundarias: recorren el árbol completo (O(n)) porque el
 * árbol está indexado por id_caso, no por fecha ni por zona.
 */
void consultar_rango_fechas(NodoAVL* raiz, const char* inicio, const char* fin);
void consultar_por_zona(NodoAVL* raiz, const char* zona);

#endif
