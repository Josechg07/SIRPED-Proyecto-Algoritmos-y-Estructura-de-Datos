/*
 * ===========================================================================
 *  lista.c
 *  Sistema de Recoleccion de Datos — Desaparecidos Terremoto Venezuela 2026
 * ---------------------------------------------------------------------------
 *  Implementacion de la lista enlazada simple.
 * ===========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

/* ── Insercion al inicio ─────────────────────────────────────────────────── */

NodoLista* listaInsertar(NodoLista *lista, Desaparecido *persona)
{
    NodoLista *nuevo = (NodoLista *)malloc(sizeof(NodoLista));
    if (!nuevo) {
        fprintf(stderr, "[ERROR] No se pudo asignar memoria para NodoLista.\n");
        return lista;
    }

    nuevo->dato = persona;
    nuevo->sig  = lista;

    return nuevo;
}

/* ── Impresion ───────────────────────────────────────────────────────────── */

void listaImprimir(NodoLista *lista)
{
    NodoLista *actual = lista;
    while (actual) {
        imprimirDesaparecido(actual->dato);
        actual = actual->sig;
    }
}

/* ── Conteo ──────────────────────────────────────────────────────────────── */

int listaContarElementos(NodoLista *lista)
{
    int conteo = 0;
    NodoLista *actual = lista;
    while (actual) {
        conteo++;
        actual = actual->sig;
    }
    return conteo;
}

/* ── Liberacion ──────────────────────────────────────────────────────────── */
/*
 * Libera SOLO los nodos de la lista, NO los datos (Desaparecido*),
 * ya que estos pueden estar compartidos entre multiples arboles.
 */

void listaLiberar(NodoLista *lista)
{
    NodoLista *actual = lista;
    NodoLista *temp;

    while (actual) {
        temp   = actual;
        actual = actual->sig;
        free(temp);
    }
}
