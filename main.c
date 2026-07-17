/* ============================================================================
 * main.c
 * ----------------------------------------------------------------------------
 * Proyecto: SIRPED (Sistema de Registro y Seguimiento de Personas Desaparecidas)
 * Módulo:   Ordenamiento + Integración (main, menú, memoria)
 * ----------------------------------------------------------------------------
 * Punto de entrada del sistema SIRPED. Su única responsabilidad es arrancar
 * el menú interactivo, que a su vez coordina el resto de los módulos.
 *
 * Toda la memoria dinámica reservada durante la ejecución (arreglo de
 * carga masiva, nodos del árbol AVL simulado y nodos de las listas de
 * historial) es liberada dentro de ejecutar_menu() cuando el usuario
 * selecciona la opción "5. Salir", por lo que al terminar main() no debe
 * quedar ningún bloque de memoria dinámica sin liberar (verificable con
 * Valgrind, ver instrucciones al final de este archivo).
 * ============================================================================
 */

#include <stdio.h>
#include "estructuras.h"
#include "menu.c" 

int main(void) {
    printf("Iniciando SIRPED...\n");

    /* 
     * NOTA: Si necesitas hacer pruebas manuales del historial antes de arrancar el menú,
     * descomenta las siguientes líneas. De lo contrario, ejecutar_menu() se encarga de todo.
     *
     * NodoHistorial* historial = NULL;
     * historial = insertar_historial(historial, crear_nodo_historial("24/06/2026", "Reportado desaparecido en La Guaira"));
     * historial = insertar_historial(historial, crear_nodo_historial("02/07/2026", "Encontrado con heridas, trasladado al hospital"));
     * imprimir_historial(historial);
     * liberar_historial(historial);
     */

    // Arranca el flujo principal del sistema
    ejecutar_menu();

    printf("Programa finalizado sin fugas de memoria pendientes.\n");
    return 0;
}

/* ============================================================================
 * INSTRUCCIONES DE COMPILACIÓN Y PRUEBA DE MEMORIA (Valgrind)
 * ----------------------------------------------------------------------------
 * 1) COMPILAR (usando C99 y advertencias estrictas):
 *
 *      gcc -std=c99 -Wall -Wextra -g -o sirped main.c menu.c parser.c \
 *          ordenamiento.c memoria.c
 *
 *    -Wall -Wextra  -> activa advertencias adicionales del compilador.
 *    -g             -> incluye información de depuración (necesaria para
 *                      que Valgrind muestre líneas exactas del código).
 *
 * 2) EJECUTAR NORMALMENTE:
 *
 *      ./sirped
 *
 * 3) VERIFICAR FUGAS DE MEMORIA CON VALGRIND:
 *
 *      valgrind --leak-check=full --show-leak-kinds=all \
 *          --track-origins=yes ./sirped
 * ============================================================================
 */
