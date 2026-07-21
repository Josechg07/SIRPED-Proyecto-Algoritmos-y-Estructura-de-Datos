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



// Renderiza la cabecera principal tipo "App"
void mostrar_header_app() {
    printf("   SIRPED v1.0                                                          \n");
    printf("  Sistema de Registro y Seguimiento de Personas Desaparecidas             \n");
}

void mostrar_footer_app() {
    printf("  Usa los numeros del 1 al 8 para navegar.   \n");
}

// Reemplazo completo de la función de menú interactivo con formato estético de App
void desplegar_menu() {
    limpiar_pantalla();
    
    // Dibujado del Dashboard / Panel de control de la App
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
    printf("      [6]    Consultar por Nombre de Persona                              \n");
    printf("      [7]    Cargar Casos desde Archivo Externo                           \n");
    printf("      [8]    Salir de la Aplicacion y Liberar Memoria                     \n");
    printf("                                                                          \n");
    printf("                                                                          \n");
    printf("                                                                          \n");
    mostrar_footer_app();
    
    printf("\n  Seleccione una opcion del menu: ");
}
