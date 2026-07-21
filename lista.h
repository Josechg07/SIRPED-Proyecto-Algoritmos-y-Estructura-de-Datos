/*
 * ===========================================================================
 *  lista.h
 *  Sistema de Recoleccion de Datos — Desaparecidos Terremoto Venezuela 2026
 * ---------------------------------------------------------------------------
 *  Lista enlazada simple para almacenar registros de desaparecidos.
 *  El nodo del arbol AVL contiene SOLO un puntero (NodoLista *) a esta
 *  lista, sin conocer su implementacion interna.
 * ===========================================================================
 */

#ifndef LISTA_H
#define LISTA_H

#include "desaparecido.h"

/* ── Nodo de la lista enlazada ───────────────────────────────────────────── */

typedef struct NodoLista {
    Desaparecido      *dato;    /* Puntero al registro de la persona       */
    struct NodoLista  *sig;     /* Puntero al siguiente nodo               */
} NodoLista;

/* ── Operaciones de la lista ─────────────────────────────────────────────── */

/*
 * Inserta una persona al inicio de la lista.
 * Retorna el nuevo inicio de la lista.
 */
NodoLista* listaInsertar(NodoLista *lista, Desaparecido *persona);

/*
 * Imprime todos los registros de la lista.
 */
void listaImprimir(NodoLista *lista);

/*
 * Cuenta la cantidad de elementos en la lista.
 */
int listaContarElementos(NodoLista *lista);

/*
 * Libera la memoria de los nodos de la lista.
 * NOTA: NO libera los Desaparecido* almacenados, ya que pueden estar
 * compartidos entre multiples arboles (arbolFechas y arbolZonas).
 */
void listaLiberar(NodoLista *lista);

#endif /* LISTA_H */
