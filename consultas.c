#include <stdio.h>
#include <string.h>
#include "consultas.h"

/*
 * Como el árbol está ordenado por id_caso, la búsqueda solo necesita
 * bajar por UN subárbol en cada paso (comparación estándar de BST/AVL):
 * O(log n), a diferencia de una búsqueda que tuviera que revisar ambos
 * subárboles porque la clave de orden fuera otra distinta al id.
 */
NodoAVL* buscar_por_id(NodoAVL* raiz, const char* id) {
    if (raiz == NULL) return NULL;

    int cmp = strcmp(id, raiz->id_caso);
    if (cmp == 0) return raiz;
    if (cmp < 0) return buscar_por_id(raiz->izquierdo, id);
    return buscar_por_id(raiz->derecho, id);
}

void mostrar_caso(NodoAVL* nodo) {
    printf("\n========================================\n");
    printf("ID CASO: %s\n", nodo->id_caso);
    printf("Persona: %s\n", nodo->nombre_persona);
    printf("Fecha Inicial: %s | Zona: %s\n", nodo->fecha_inicial, nodo->zona_geografica);
    printf("----------------------------------------\n");
    printf("HISTORIAL CRONOLÓGICO DE ESTADOS:\n");

    NodoHistorial* h = nodo->historial;
    int paso = 1;
    while (h != NULL) {
        printf("  [%d] Fecha: %s | Ubicación: %s | Estado: %s\n", paso++, h->fecha_actualizacion, h->ubicacion, h->estado);
        printf("      Descripción: %s\n", h->descripcion);
        h = h->siguiente;
    }
    printf("========================================\n");
}

/*
 * Recorrido acotado por rango de fechas. El árbol NO está ordenado por
 * fecha (está ordenado por id_caso), así que esta función recorre TODO
 * el árbol (O(n)) filtrando por fecha_inicial. Se documenta así en el
 * informe técnico: es la consecuencia esperada de haber priorizado que
 * la operación central (buscar/actualizar por id) sea O(log n).
 */
void consultar_rango_fechas(NodoAVL* raiz, const char* inicio, const char* fin) {
    if (raiz == NULL) return;

    consultar_rango_fechas(raiz->izquierdo, inicio, fin);
    if (strcmp(raiz->fecha_inicial, inicio) >= 0 && strcmp(raiz->fecha_inicial, fin) <= 0) {
        mostrar_caso(raiz);
    }
    consultar_rango_fechas(raiz->derecho, inicio, fin);
}

/* Igual que arriba: recorrido completo O(n), filtrando por zona. */
void consultar_por_zona(NodoAVL* raiz, const char* zona) {
    if (raiz == NULL) return;
    consultar_por_zona(raiz->izquierdo, zona);
    if (strstr(raiz->zona_geografica, zona) != NULL) {
        mostrar_caso(raiz);
    }
    consultar_por_zona(raiz->derecho, zona);
}

/* Recorrido completo O(n), filtrando por nombre de persona usando substring. */
void consultar_por_nombre(NodoAVL* raiz, const char* nombre) {
    if (raiz == NULL) return;
    consultar_por_nombre(raiz->izquierdo, nombre);
    if (strstr(raiz->nombre_persona, nombre) != NULL) {
        mostrar_caso(raiz);
    }
    consultar_por_nombre(raiz->derecho, nombre);
}
