#include <stdio.h>
#include "estructuras.h"

int main() {
    NodoHistorial* historial = NULL;

    historial = insertar_historial(historial, crear_nodo_historial("24/06/2026", "Reportado desaparecido en La Guaira"));
    historial = insertar_historial(historial, crear_nodo_historial("02/07/2026", "Encontrado con heridas, trasladado al hospital"));

    imprimir_historial(historial);

    liberar_historial(historial);
    return 0;
}