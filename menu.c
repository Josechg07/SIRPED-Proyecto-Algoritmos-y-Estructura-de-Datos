#include <stdio.h>
#include <stdlib.h>

/* Función auxiliar para limpiar la pantalla según el sistema operativo */
static void limpiar_pantalla(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Ya no se usa una pausa de entrada; el programa debe arrancar y
 * mostrar el menú directamente sin esperar a que el usuario presione ENTER. */
void esperar_enter(void) {
    (void)0;
}

static void mostrar_header_app(void) {
    printf("   SIRPED v1.0                                                          \n");
    printf("  Sistema de Registro y Seguimiento de Personas Desaparecidas             \n");
}

static void mostrar_footer_app(void) {
    printf("  Usa los numeros del 1 al 7 para navegar.   \n");
}

void desplegar_menu(void) {
    limpiar_pantalla();

    mostrar_header_app();
    printf("                                                                          \n");
    printf("                                                                          \n");
    printf("                       PANEL DE CONTROL PRINCIPAL                         \n");
    printf("                                                                          \n");
    printf("                                                                          \n");
    printf("      [1]    Registrar Nuevo Caso                                         \n");
    printf("      [2]    Actualizar Estado                                            \n");
    printf("      [3]    Consultar por Rango de Fechas                                \n");
    printf("      [4]    Consultar por Zona Geografica                                \n");
    printf("      [5]    Buscar Caso Individual por ID Completo                       \n");
    printf("      [6]    Cargar Reportes Masivos desde Archivo .txt                    \n");
    printf("      [7]    Salir de la Aplicacion y Liberar Memoria                      \n");
    printf("                                                                          \n");
    printf("                                                                          \n");
    printf("                                                                          \n");
    printf("                                                                          \n");
    printf("                                                                          \n");
    mostrar_footer_app();

    printf("\n  Seleccione una opcion del menu: ");
}