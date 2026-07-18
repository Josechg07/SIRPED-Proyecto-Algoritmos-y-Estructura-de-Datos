#include <stdlib.h>
#include "estructuras.h"
#include "lista_historial.h"
#include "memoria.h"

void liberar_sistema(NodoAVL* raiz) {
    if (raiz == NULL) return;
    liberar_sistema(raiz->izquierdo);
    liberar_sistema(raiz->derecho);
    liberar_historial(raiz->historial);
    free(raiz);
}
