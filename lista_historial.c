#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"

NodoHistorial* crear_nodo_historial(const char* fecha, const char* descripcion) {
    NodoHistorial* nuevo = (NodoHistorial*) malloc(sizeof(NodoHistorial));
    if (nuevo == NULL) {
        return NULL;
    }

    strncpy(nuevo->fecha, fecha, 10);
    nuevo->fecha[10] = '\0';

    strncpy(nuevo->descripcion, descripcion, MAX_DESCRIPCION - 1);
    nuevo->descripcion[MAX_DESCRIPCION - 1] = '\0';

    nuevo->siguiente = NULL;

    return nuevo;
}

NodoHistorial* insertar_historial(NodoHistorial* cabeza, NodoHistorial* nuevo) {
    if (cabeza == NULL) {
        return nuevo;
    }

    NodoHistorial* actual = cabeza;
    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }
    actual->siguiente = nuevo;

    return cabeza;
}

void imprimir_historial(NodoHistorial* cabeza) {
    NodoHistorial* actual = cabeza;
    while (actual != NULL) {
        printf("[%s] %s\n", actual->fecha, actual->descripcion);
        actual = actual->siguiente;
    }
}

void liberar_historial(NodoHistorial* cabeza) {
    NodoHistorial* actual = cabeza;
    while (actual != NULL) {
        NodoHistorial* siguiente = actual->siguiente;
        free(actual);
        actual = siguiente;
    }
}