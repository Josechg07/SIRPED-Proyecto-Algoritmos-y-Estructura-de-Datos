#include <stdio.h>
#include <stdlib.h>

// Función auxiliar para limpiar la pantalla según el sistema operativo
void limpiar_pantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Dibuja una línea horizontal con caracteres de recuadro dobles
void dibujar_linea_doble(int ancho) {
    for (int i = 0; i < ancho; i++) {
        printf("═");
    }
}

// Renderiza la cabecera principal tipo "App"
void mostrar_header_app() {
    printf("╔"); dibujar_linea_doble(76); printf("╗\n");
    printf("║  » SIRPED v1.0 «                                                         ║\n");
    printf("║  Sistema de Registro y Seguimiento de Personas Desaparecidas             ║\n");
    printf("╠"); dibujar_linea_doble(76); printf("╣\n");
}

// Renderiza el pie de página de la aplicación
void mostrar_footer_app() {
    printf("╠"); dibujar_linea_doble(76); printf("╣\n");
    printf("║  [Monteávila Ingeniería]  -  Usa los números del 1 al 6 para navegar.   ║\n");
    printf("╚"); dibujar_linea_doble(76); printf("╝\n");
}

// Reemplazo completo de la función de menú interactivo con formato estético de App
void desplegar_menu() {
    limpiar_pantalla();
    
    // Dibujado del Dashboard / Panel de control de la App
    mostrar_header_app();
    printf("║                                                                          ║\n");
    printf("║   ┌──────────────────────────────────────────────────────────────────┐   ║\n");
    printf("║   │                    PANEL DE CONTROL PRINCIPAL                    │   ║\n");
    printf("║   └──────────────────────────────────────────────────────────────────┘   ║\n");
    printf("║                                                                          ║\n");
    printf("║      [1]  📝  Registrar Nuevo Caso (Indexación Árbol AVL)               ║\n");
    printf("║      [2]  🔄  Actualizar Estado (Historial Cronológico)                 ║\n");
    printf("║      [3]  📅  Consultar por Rango de Fechas (Búsqueda Eficiente)         ║\n");
    printf("║      [4]  📍  Consultar por Zona Geográfica                             ║\n");
    printf("║      [5]  🔍  Buscar Caso Individual por ID Completo                    ║\n");
    printf("║      [6]  ❌  Salir de la Aplicación y Liberar Memoria                  ║\n");
    printf("║                                                                          ║\n");
    printf("║   ┌──────────────────────────────────────────────────────────────────┐   ║\n");
    printf("║   │  Estado del Motor Core C: ACTIVO | Optimización AVL: O(log n)    │   ║\n");
    printf("║   └──────────────────────────────────────────────────────────────────┘   ║\n");
    printf("║                                                                          ║\n");
    mostrar_footer_app();
    
    printf("\n ❯ Seleccione una opción del menú: ");
}
