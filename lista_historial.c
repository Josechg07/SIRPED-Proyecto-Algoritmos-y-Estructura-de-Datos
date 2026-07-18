#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista_historial.h"

NodoHistorial* crear_nodo_historial(const char* fecha, const char* ubica, const char* est, const char* desc) {
    NodoHistorial* nuevo = (NodoHistorial*)malloc(sizeof(NodoHistorial));
    if (!nuevo) {
        fprintf(stderr, "[ERROR] No se pudo reservar memoria para NodoHistorial.\n");
        return NULL;
    }
    strncpy(nuevo->fecha_actualizacion, fecha, 10);
    nuevo->fecha_actualizacion[10] = '\0';
    strncpy(nuevo->ubicacion, ubica, 99);
    nuevo->ubicacion[99] = '\0';
    strncpy(nuevo->estado, est, 49);
    nuevo->estado[49] = '\0';
    strncpy(nuevo->descripcion, desc, 255);
    nuevo->descripcion[255] = '\0';
    nuevo->siguiente = NULL;
    return nuevo;
}

void agregar_historial(NodoAVL* nodo_caso, const char* fecha, const char* ubica, const char* est, const char* desc) {
    NodoHistorial* nuevo = crear_nodo_historial(fecha, ubica, est, desc);
    if (!nuevo) {
        return; /* malloc falló; se avisó por stderr en crear_nodo_historial */
    }
    if (!nodo_caso->historial) {
        nodo_caso->historial = nuevo;
    } else {
        NodoHistorial* actual = nodo_caso->historial;
        while (actual->siguiente != NULL) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
}

void liberar_historial(NodoHistorial* h) {
    while (h != NULL) {
        NodoHistorial* temp = h;
        h = h->siguiente;
        free(temp);
    }
}
