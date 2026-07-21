/*
 * ===========================================================================
 *  arbol_avl.c
 *  Sistema de Recoleccion de Datos — Desaparecidos Terremoto Venezuela 2026
 * ---------------------------------------------------------------------------
 *  Implementacion del arbol AVL con insercion balanceada,
 *  busqueda exacta, busqueda por rango y recorrido in-order.
 *
 *  Rotaciones implementadas:
 *    - Rotacion simple a la derecha  (caso LL)
 *    - Rotacion simple a la izquierda (caso RR)
 *    - Rotacion doble izquierda-derecha (caso LR)
 *    - Rotacion doble derecha-izquierda (caso RL)
 * ===========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbol_avl.h"
#include "lista.h"          /* Aqui si incluimos la implementacion completa */
#include "desaparecido.h"   /* Para operar con los datos de la persona     */

/* ═══════════════════════════════════════════════════════════════════════════
 *  FUNCIONES INTERNAS (static)
 * ═══════════════════════════════════════════════════════════════════════════ */

/* ── Obtener altura de un nodo (0 si es NULL) ────────────────────────────── */

static int altura(NodoAVL *nodo)
{
    return nodo ? nodo->altura : 0;
}

/* ── Maximo entre dos enteros ────────────────────────────────────────────── */

static int maximo(int a, int b)
{
    return (a > b) ? a : b;
}

/* ── Factor de balance: altura(izq) - altura(der) ───────────────────────── */

static int factorBalance(NodoAVL *nodo)
{
    return nodo ? altura(nodo->izq) - altura(nodo->der) : 0;
}

/* ── Crear un nuevo nodo AVL ─────────────────────────────────────────────── */

static NodoAVL* crearNodo(const char *clave)
{
    NodoAVL *nodo = (NodoAVL *)malloc(sizeof(NodoAVL));
    if (!nodo) {
        fprintf(stderr, "[ERROR] No se pudo asignar memoria para NodoAVL.\n");
        return NULL;
    }

    strncpy(nodo->clave, clave, 63);
    nodo->clave[63] = '\0';
    nodo->lista  = NULL;
    nodo->izq    = NULL;
    nodo->der    = NULL;
    nodo->altura = 1;   /* Un nodo hoja tiene altura 1 */

    return nodo;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  ROTACIONES AVL
 * ═══════════════════════════════════════════════════════════════════════════ */

/*
 *  Rotacion simple a la derecha (caso Left-Left):
 *
 *        y                x
 *       / \              / \
 *      x   T3    →     T1   y
 *     / \                  / \
 *   T1   T2              T2   T3
 */
static NodoAVL* rotarDerecha(NodoAVL *y)
{
    NodoAVL *x  = y->izq;
    NodoAVL *T2 = x->der;

    /* Realizar la rotacion */
    x->der = y;
    y->izq = T2;

    /* Actualizar alturas (y primero, porque ahora es hijo de x) */
    y->altura = 1 + maximo(altura(y->izq), altura(y->der));
    x->altura = 1 + maximo(altura(x->izq), altura(x->der));

    return x;   /* x es la nueva raiz del subarbol */
}

/*
 *  Rotacion simple a la izquierda (caso Right-Right):
 *
 *      x                  y
 *     / \                / \
 *   T1   y      →      x   T3
 *       / \            / \
 *     T2   T3        T1   T2
 */
static NodoAVL* rotarIzquierda(NodoAVL *x)
{
    NodoAVL *y  = x->der;
    NodoAVL *T2 = y->izq;

    /* Realizar la rotacion */
    y->izq = x;
    x->der = T2;

    /* Actualizar alturas (x primero, porque ahora es hijo de y) */
    x->altura = 1 + maximo(altura(x->izq), altura(x->der));
    y->altura = 1 + maximo(altura(y->izq), altura(y->der));

    return y;   /* y es la nueva raiz del subarbol */
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  INSERCION CON BALANCEO AUTOMATICO
 * ═══════════════════════════════════════════════════════════════════════════ */

NodoAVL* avlInsertar(NodoAVL *raiz, const char *clave,
                     struct Desaparecido *persona)
{
    /* ── Paso 1: Insercion BST estandar ──────────────────────────────────── */

    if (!raiz) {
        NodoAVL *nodo = crearNodo(clave);
        if (!nodo) return NULL;
        nodo->lista = listaInsertar(NULL, persona);
        return nodo;
    }

    int cmp = strcmp(clave, raiz->clave);

    if (cmp < 0) {
        raiz->izq = avlInsertar(raiz->izq, clave, persona);
    } else if (cmp > 0) {
        raiz->der = avlInsertar(raiz->der, clave, persona);
    } else {
        /*
         * La clave ya existe: la persona se agrega a la lista
         * enlazada existente del nodo. No se crea un nodo nuevo.
         */
        raiz->lista = listaInsertar(raiz->lista, persona);
        return raiz;   /* No hay cambio de estructura, no se necesita rebalancear */
    }

    /* ── Paso 2: Actualizar altura del nodo actual ───────────────────────── */

    raiz->altura = 1 + maximo(altura(raiz->izq), altura(raiz->der));

    /* ── Paso 3: Obtener factor de balance ───────────────────────────────── */

    int balance = factorBalance(raiz);

    /* ── Paso 4: Rebalancear si es necesario (4 casos) ───────────────────── */

    /* Caso Left-Left (LL): balance > 1 y la clave fue a la izquierda-izquierda */
    if (balance > 1 && strcmp(clave, raiz->izq->clave) < 0) {
        return rotarDerecha(raiz);
    }

    /* Caso Right-Right (RR): balance < -1 y la clave fue a la derecha-derecha */
    if (balance < -1 && strcmp(clave, raiz->der->clave) > 0) {
        return rotarIzquierda(raiz);
    }

    /* Caso Left-Right (LR): balance > 1 y la clave fue a la izquierda-derecha */
    if (balance > 1 && strcmp(clave, raiz->izq->clave) > 0) {
        raiz->izq = rotarIzquierda(raiz->izq);
        return rotarDerecha(raiz);
    }

    /* Caso Right-Left (RL): balance < -1 y la clave fue a la derecha-izquierda */
    if (balance < -1 && strcmp(clave, raiz->der->clave) < 0) {
        raiz->der = rotarDerecha(raiz->der);
        return rotarIzquierda(raiz);
    }

    /* El nodo esta balanceado, retornar sin cambios */
    return raiz;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BUSQUEDA EXACTA
 * ═══════════════════════════════════════════════════════════════════════════ */

NodoAVL* avlBuscar(NodoAVL *raiz, const char *clave)
{
    if (!raiz) return NULL;

    int cmp = strcmp(clave, raiz->clave);

    if (cmp < 0) return avlBuscar(raiz->izq, clave);
    if (cmp > 0) return avlBuscar(raiz->der, clave);

    return raiz;   /* Encontrado */
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  BUSQUEDA POR RANGO [inicio, fin]
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *  Recorrido in-order acotado: solo visita subarboles que pueden
 *  contener claves dentro del rango. Esto da complejidad O(log n + k).
 *
 *  Para fechas en formato YYYY-MM-DD, la comparacion lexicografica
 *  preserva el orden cronologico, lo cual permite usar strcmp directamente.
 */

void avlBuscarRango(NodoAVL *raiz, const char *inicio, const char *fin,
                    void (*callback)(NodoAVL *nodo))
{
    if (!raiz) return;

    /*
     * Solo explorar el subarbol izquierdo si la clave actual
     * es MAYOR que el inicio del rango (podria haber resultados a la izq)
     */
    if (strcmp(raiz->clave, inicio) > 0) {
        avlBuscarRango(raiz->izq, inicio, fin, callback);
    }

    /*
     * Procesar el nodo actual si su clave esta dentro del rango [inicio, fin]
     */
    if (strcmp(raiz->clave, inicio) >= 0 && strcmp(raiz->clave, fin) <= 0) {
        callback(raiz);
    }

    /*
     * Solo explorar el subarbol derecho si la clave actual
     * es MENOR que el fin del rango (podria haber resultados a la der)
     */
    if (strcmp(raiz->clave, fin) < 0) {
        avlBuscarRango(raiz->der, inicio, fin, callback);
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  RECORRIDO IN-ORDER
 * ═══════════════════════════════════════════════════════════════════════════ */

void avlInOrder(NodoAVL *raiz, void (*callback)(NodoAVL *nodo))
{
    if (!raiz) return;

    avlInOrder(raiz->izq, callback);
    callback(raiz);
    avlInOrder(raiz->der, callback);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  CONTEO
 * ═══════════════════════════════════════════════════════════════════════════ */

int avlContarNodos(NodoAVL *raiz)
{
    if (!raiz) return 0;
    return 1 + avlContarNodos(raiz->izq) + avlContarNodos(raiz->der);
}

int avlContarPersonas(NodoAVL *raiz)
{
    if (!raiz) return 0;
    return listaContarElementos(raiz->lista)
         + avlContarPersonas(raiz->izq)
         + avlContarPersonas(raiz->der);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  UTILIDADES
 * ═══════════════════════════════════════════════════════════════════════════ */

int avlObtenerAltura(NodoAVL *raiz)
{
    return raiz ? raiz->altura : 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *  LIBERACION DE MEMORIA
 * ═══════════════════════════════════════════════════════════════════════════ */

void avlLiberar(NodoAVL *raiz)
{
    if (!raiz) return;

    avlLiberar(raiz->izq);
    avlLiberar(raiz->der);
    listaLiberar(raiz->lista);   /* Libera nodos de lista, NO los datos */
    free(raiz);
}
