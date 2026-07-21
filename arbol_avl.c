/* ============================================================================
 * arbol_avl.c
 * ----------------------------------------------------------------------------
 * Árbol AVL (auto-balanceado) indexado por id_caso (clave única de cada
 * caso). Se eligió el id como clave -y no la fecha ni el nombre- porque:
 *   - Es único por caso, así que nunca hay ambigüedad al buscar/actualizar.
 *   - La operación más frecuente del sistema (registrar, actualizar y
 *     buscar un caso puntual) queda en O(log n).
 *   - Evita el riesgo de fusionar por error el historial de dos personas
 *     distintas que compartan el mismo nombre.
 * Las consultas por rango de fechas y por zona (ordenamiento.c) recorren
 * el árbol completo -O(n)- porque esos campos no son la clave del árbol;
 * es una decisión consciente y así se documenta en el informe técnico.
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arbol_avl.h"
#include "lista_historial.h"

static int max_val(int a, int b) { return (a > b) ? a : b; }

static int obtener_altura(NodoAVL* n) {
    return (n == NULL) ? 0 : n->altura;
}

static int obtener_balance(NodoAVL* n) {
    return (n == NULL) ? 0 : obtener_altura(n->izquierdo) - obtener_altura(n->derecho);
}

/*
 *  Rotacion simple a la derecha (caso Izquierda-Izquierda):
 *        y                x
 *       / \              / \
 *      x   T3    →     T1   y
 *     / \                  / \
 *   T1   T2              T2   T3
 */
static NodoAVL* rotar_derecha(NodoAVL* y) {
    NodoAVL* x = y->izquierdo;
    NodoAVL* t2 = x->derecho;

    x->derecho = y;
    y->izquierdo = t2;

    y->altura = 1 + max_val(obtener_altura(y->izquierdo), obtener_altura(y->derecho));
    x->altura = 1 + max_val(obtener_altura(x->izquierdo), obtener_altura(x->derecho));

    return x;
}

/*
 *  Rotacion simple a la izquierda (caso Derecha-Derecha):
 *      x                  y
 *     / \                / \
 *   T1   y      →      x   T3
 *       / \            / \
 *     T2   T3        T1   T2
 */
static NodoAVL* rotar_izquierda(NodoAVL* x) {
    NodoAVL* y = x->derecho;
    NodoAVL* t2 = y->izquierdo;

    y->izquierdo = x;
    x->derecho = t2;

    x->altura = 1 + max_val(obtener_altura(x->izquierdo), obtener_altura(x->derecho));
    y->altura = 1 + max_val(obtener_altura(y->izquierdo), obtener_altura(y->derecho));

    return y;
}

NodoAVL* insertar_caso(NodoAVL* nodo, const char* id, const char* nombre,
                        const char* fecha, const char* zona,
                        const char* est, const char* desc) {
    if (nodo == NULL) {
        NodoAVL* nuevo = (NodoAVL*)malloc(sizeof(NodoAVL));
        if (!nuevo) {
            fprintf(stderr, "[ERROR] No se pudo reservar memoria para NodoAVL.\n");
            return NULL;
        }
        strncpy(nuevo->id_caso, id, 19);
        nuevo->id_caso[19] = '\0';
        strncpy(nuevo->nombre_persona, nombre, 99);
        nuevo->nombre_persona[99] = '\0';
        strncpy(nuevo->fecha_inicial, fecha, 10);
        nuevo->fecha_inicial[10] = '\0';
        strncpy(nuevo->zona_geografica, zona, 99);
        nuevo->zona_geografica[99] = '\0';
        nuevo->altura = 1;
        nuevo->izquierdo = NULL;
        nuevo->derecho = NULL;
        nuevo->historial = NULL;

        agregar_historial(nuevo, fecha, zona, est, desc);
        return nuevo;
    }

    int cmp = strcmp(id, nodo->id_caso);

    if (cmp < 0) {
        nodo->izquierdo = insertar_caso(nodo->izquierdo, id, nombre, fecha, zona, est, desc);
    } else if (cmp > 0) {
        nodo->derecho = insertar_caso(nodo->derecho, id, nombre, fecha, zona, est, desc);
    } else {
        /* id repetido: no debería pasar si el id se genera bien, pero por
         * seguridad no se crea un nodo duplicado; se agrega al historial. */
        agregar_historial(nodo, fecha, zona, est, desc);
        return nodo;
    }

    nodo->altura = 1 + max_val(obtener_altura(nodo->izquierdo), obtener_altura(nodo->derecho));
    int balance = obtener_balance(nodo);

    if (balance > 1 && strcmp(id, nodo->izquierdo->id_caso) < 0)
        return rotar_derecha(nodo);

    if (balance < -1 && strcmp(id, nodo->derecho->id_caso) > 0)
        return rotar_izquierda(nodo);

    if (balance > 1 && strcmp(id, nodo->izquierdo->id_caso) > 0) {
        nodo->izquierdo = rotar_izquierda(nodo->izquierdo);
        return rotar_derecha(nodo);
    }

    if (balance < -1 && strcmp(id, nodo->derecho->id_caso) < 0) {
        nodo->derecho = rotar_derecha(nodo->derecho);
        return rotar_izquierda(nodo);
    }

    return nodo;
}
