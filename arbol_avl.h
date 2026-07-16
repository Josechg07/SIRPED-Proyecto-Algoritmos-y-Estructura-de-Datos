/*
 * ===========================================================================
 *  arbol_avl.h
 *  Sistema de Recoleccion de Datos — Desaparecidos Terremoto Venezuela 2026
 * ---------------------------------------------------------------------------
 *  Arbol AVL (auto-balanceado) para indexar registros de desaparecidos.
 *
 *  IMPORTANTE: El nodo del arbol contiene SOLO UN PUNTERO a la lista
 *  enlazada (struct NodoLista *), sin incluir la implementacion completa
 *  de la lista. La lista se implementa en lista.h / lista.c.
 *
 *  Se utilizan dos arboles AVL independientes:
 *    1. ArbolFechas — clave: fecha (YYYY-MM-DD), busqueda por rango
 *    2. ArbolZonas  — clave: nombre de zona, busqueda exacta
 * ===========================================================================
 */

#ifndef ARBOL_AVL_H
#define ARBOL_AVL_H

/* ── Forward declarations ────────────────────────────────────────────────── */
/*
 * Solo declaramos que estas estructuras existen. No incluimos sus headers.
 * El nodo del arbol maneja SOLO PUNTEROS a estos tipos.
 */
struct NodoLista;
struct Desaparecido;

/* ── Nodo del arbol AVL ──────────────────────────────────────────────────── */

typedef struct NodoAVL {
    char               clave[64];  /* Clave de busqueda (fecha o zona)      */
    struct NodoLista  *lista;      /* >>> SOLO UN PUNTERO a la lista <<<    */
    struct NodoAVL    *izq;        /* Hijo izquierdo                        */
    struct NodoAVL    *der;        /* Hijo derecho                          */
    int                altura;     /* Altura del nodo para balanceo AVL     */
} NodoAVL;

/* ── Operaciones del arbol AVL ───────────────────────────────────────────── */

/*
 * Inserta un registro en el arbol con balanceo automatico.
 * Si la clave ya existe, la persona se agrega a la lista existente.
 * Si la clave es nueva, se crea un nuevo nodo con una nueva lista.
 * Retorna la nueva raiz del arbol (puede cambiar por rotaciones).
 */
NodoAVL* avlInsertar(NodoAVL *raiz, const char *clave,
                     struct Desaparecido *persona);

/*
 * Busqueda exacta por clave. Retorna el nodo o NULL si no se encuentra.
 * Complejidad: O(log n)
 */
NodoAVL* avlBuscar(NodoAVL *raiz, const char *clave);

/*
 * Busqueda por rango de claves [inicio, fin] (inclusive).
 * Recorre el arbol visitando SOLO los nodos cuya clave esta en el rango.
 * Invoca el callback para cada nodo encontrado.
 * Complejidad: O(log n + k), donde k = nodos en el rango.
 *
 * Ideal para busqueda por rango de fechas:
 *   avlBuscarRango(arbolFechas, "2026-06-10", "2026-06-20", callback);
 */
void avlBuscarRango(NodoAVL *raiz, const char *inicio, const char *fin,
                    void (*callback)(NodoAVL *nodo));

/*
 * Recorrido in-order (orden ascendente por clave).
 * Invoca el callback para cada nodo.
 */
void avlInOrder(NodoAVL *raiz, void (*callback)(NodoAVL *nodo));

/*
 * Cuenta el numero total de nodos en el arbol.
 */
int avlContarNodos(NodoAVL *raiz);

/*
 * Cuenta el numero total de personas en todas las listas del arbol.
 */
int avlContarPersonas(NodoAVL *raiz);

/*
 * Obtiene la altura del arbol (0 si esta vacio).
 */
int avlObtenerAltura(NodoAVL *raiz);

/*
 * Libera toda la memoria del arbol (nodos AVL + nodos de lista).
 * NOTA: NO libera los Desaparecido* almacenados en las listas.
 */
void avlLiberar(NodoAVL *raiz);

#endif /* ARBOL_AVL_H */
