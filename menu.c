/* ============================================================================
 * menu.c
 * ----------------------------------------------------------------------------
 * Implementa el menú interactivo por consola que integra todos los módulos:
 * lectura de archivo (parser.c), ordenamiento (ordenamiento.c), inserción
 * simulada en el AVL y en las listas de historial (memoria.c), y liberación
 * final de toda la memoria al salir.
 *
 * ESTADO DEL PROGRAMA (variables estáticas de este archivo):
 *   Se mantienen a nivel de archivo (static) en vez de usar variables
 *   globales "sueltas" en main.c, para encapsular el estado del sistema
 *   dentro del módulo que lo administra. main.c solo llama a
 *   ejecutar_menu() y no necesita conocer estos detalles internos.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"

/* ---- Estado global del sistema, encapsulado en este archivo ---- */
static NodoAVL *raiz_avl = NULL;      /* raíz del árbol (mock) de reportes  */
static Reporte *buffer_carga = NULL;  /* último arreglo cargado desde .txt  */
static int buffer_cantidad = 0;
static int buffer_capacidad = 0;
static int siguiente_id_manual = 100000; /* ids altos para no chocar con los del .txt */

/* ----------------------------------------------------------------------
 * limpiar_buffer_entrada
 * Descarta el resto de caracteres pendientes en stdin hasta el siguiente
 * salto de línea (incluyéndolo). Necesario después de usar scanf("%d",..)
 * para que un fgets() posterior no lea una línea vacía "sobrante".
 * ---------------------------------------------------------------------- */
static void limpiar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descartar */
    }
}

/* ----------------------------------------------------------------------
 * leer_linea
 * Lee una línea de texto desde stdin de forma segura hacia un buffer de
 * tamaño fijo, eliminando el salto de línea final.
 * ---------------------------------------------------------------------- */
static void leer_linea(char *destino, size_t tam) {
    if (fgets(destino, (int) tam, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        }
    } else {
        destino[0] = '\0';
    }
}

/* ----------------------------------------------------------------------
 * liberar_carga_previa
 * Si ya había un arreglo de reportes cargado en memoria de una carga
 * anterior, lo libera antes de reemplazarlo, evitando así una fuga por
 * "reasignación sin liberar" (perder la única referencia a un bloque
 * de memoria previamente reservado).
 * ---------------------------------------------------------------------- */
static void liberar_carga_previa(void) {
    if (buffer_carga != NULL) {
        liberar_arreglo_reportes(&buffer_carga);
        buffer_cantidad = 0;
        buffer_capacidad = 0;
    }
}

/* ----------------------------------------------------------------------
 * opcion_cargar_masivo
 * Opción 1 del menú: pide la ruta del .txt, lo lee, ordena los reportes
 * cronológicamente con Merge Sort y luego los inserta uno a uno en el
 * árbol (mock) para simular la construcción del índice.
 * ---------------------------------------------------------------------- */
static void opcion_cargar_masivo(void) {
    char ruta[256];
    int i;

    printf("\nRuta del archivo .txt a cargar (ej. reportes.txt): ");
    leer_linea(ruta, sizeof(ruta));

    liberar_carga_previa(); /* evita fuga si ya había una carga anterior */

    buffer_carga = cargar_reportes_desde_archivo(ruta, &buffer_cantidad, &buffer_capacidad);

    if (buffer_carga == NULL || buffer_cantidad == 0) {
        printf("No se cargó ningún reporte válido.\n");
        return;
    }

    printf("Se leyeron %d reportes desde el archivo.\n", buffer_cantidad);

    /* Ordenamiento cronológico con Merge Sort: O(n log n) garantizado */
    merge_sort_reportes(buffer_carga, 0, buffer_cantidad - 1);
    printf("Reportes ordenados cronológicamente (Merge Sort).\n");

    /* Inserción simulada en el árbol AVL (mock) */
    for (i = 0; i < buffer_cantidad; i++) {
        raiz_avl = mock_insertar_avl(raiz_avl, buffer_carga[i]);
    }
    printf("Los %d reportes fueron insertados en el índice (AVL simulado).\n", buffer_cantidad);
    printf("Total de casos en el sistema: %d\n", contar_nodos_avl(raiz_avl));
}

/* ----------------------------------------------------------------------
 * opcion_registro_manual
 * Opción 2 del menú: solicita los datos de un único reporte por consola
 * y lo inserta directamente en el árbol (no requiere ordenamiento porque
 * es un único elemento).
 * ---------------------------------------------------------------------- */
static void opcion_registro_manual(void) {
    Reporte nuevo;
    char texto_fecha[32];

    printf("\n--- Registro manual de reporte ---\n");

    nuevo.id = siguiente_id_manual++;

    printf("Nombre completo: ");
    leer_linea(nuevo.nombre, MAX_NOMBRE);

    printf("Fecha del reporte (dd/mm/aaaa): ");
    leer_linea(texto_fecha, sizeof(texto_fecha));
    if (sscanf(texto_fecha, "%d/%d/%d", &nuevo.fecha_reporte.dia,
               &nuevo.fecha_reporte.mes, &nuevo.fecha_reporte.anio) != 3) {
        printf("Formato de fecha inválido. Se cancela el registro.\n");
        return;
    }

    printf("Ubicación: ");
    leer_linea(nuevo.ubicacion, MAX_UBICACION);

    printf("Estado (ej. Buscando/Encontrado/Cerrado): ");
    leer_linea(nuevo.estado, MAX_ESTADO);

    printf("Descripción: ");
    leer_linea(nuevo.descripcion, MAX_DESCRIPCION);

    raiz_avl = mock_insertar_avl(raiz_avl, nuevo);

    printf("Reporte registrado con ID %d.\n", nuevo.id);
}

/* ----------------------------------------------------------------------
 * opcion_consultar_historial
 * Opción 3 del menú: busca un caso por ID en el árbol y recorre su
 * ListaHistorial mostrando todos los cambios de estado registrados.
 * ---------------------------------------------------------------------- */
static void opcion_consultar_historial(void) {
    int id;
    NodoAVL *nodo;
    NodoHistorial *cursor;

    printf("\nID del caso a consultar: ");
    if (scanf("%d", &id) != 1) {
        printf("Entrada inválida.\n");
        limpiar_buffer_entrada();
        return;
    }
    limpiar_buffer_entrada();

    nodo = buscar_por_id(raiz_avl, id);
    if (nodo == NULL) {
        printf("No se encontró ningún caso con ID %d.\n", id);
        return;
    }

    printf("\n--- Historial de %s (ID %d) ---\n", nodo->dato.nombre, nodo->dato.id);
    cursor = nodo->historial->cabeza;
    if (cursor == NULL) {
        printf("  (sin historial registrado)\n");
    }
    while (cursor != NULL) {
        printf("  [%02d/%02d/%04d] Estado: %-15s Detalle: %s\n",
               cursor->fecha_cambio.dia, cursor->fecha_cambio.mes,
               cursor->fecha_cambio.anio, cursor->estado, cursor->descripcion);
        cursor = cursor->siguiente;
    }
}

/* ----------------------------------------------------------------------
 * opcion_mostrar_estadisticas
 * Opción 4 del menú: muestra el total de casos y el listado completo
 * de reportes ordenados cronológicamente (recorrido InOrder del árbol).
 * ---------------------------------------------------------------------- */
static void opcion_mostrar_estadisticas(void) {
    int total = contar_nodos_avl(raiz_avl);

    printf("\n--- Estadísticas del sistema ---\n");
    printf("Total de casos registrados: %d\n", total);

    if (total == 0) {
        printf("No hay reportes que mostrar todavía.\n");
        return;
    }

    printf("\nListado cronológico de reportes:\n");
    mostrar_reportes_inorder(raiz_avl);
}

/* ----------------------------------------------------------------------
 * mostrar_opciones
 * Imprime el menú de opciones disponibles.
 * ---------------------------------------------------------------------- */
static void mostrar_opciones(void) {
    printf("\n==========================================\n");
    printf("   SIRPED - Sistema de Registro y\n");
    printf("   Seguimiento de Personas Desaparecidas\n");
    printf("==========================================\n");
    printf(" 1. Cargar reportes masivos desde archivo .txt\n");
    printf(" 2. Registrar un reporte de forma manual\n");
    printf(" 3. Consultar historial de una persona (por ID)\n");
    printf(" 4. Mostrar estadísticas / reportes ordenados\n");
    printf(" 5. Salir (y liberar memoria)\n");
    printf("==========================================\n");
    printf("Seleccione una opción: ");
}

/* ----------------------------------------------------------------------
 * ejecutar_menu
 * Bucle principal del programa: muestra el menú, lee la opción elegida
 * y despacha a la función correspondiente, hasta que el usuario elige
 * salir. Al salir, libera TODA la memoria dinámica pendiente.
 * ---------------------------------------------------------------------- */
void ejecutar_menu(void) {
    int opcion;
    int continuar = 1;

    while (continuar) {
        mostrar_opciones();

        if (scanf("%d", &opcion) != 1) {
            printf("Entrada inválida, intente de nuevo.\n");
            limpiar_buffer_entrada();
            continue;
        }
        limpiar_buffer_entrada();

        switch (opcion) {
            case 1:
                opcion_cargar_masivo();
                break;
            case 2:
                opcion_registro_manual();
                break;
            case 3:
                opcion_consultar_historial();
                break;
            case 4:
                opcion_mostrar_estadisticas();
                break;
            case 5:
                printf("\nLiberando memoria y cerrando el sistema...\n");

                /* Liberamos el árbol completo (que a su vez libera cada
                 * ListaHistorial colgada de sus nodos) */
                liberar_arbol_avl(&raiz_avl);

                /* Liberamos el último arreglo de carga masiva, si quedó
                 * alguno reservado */
                liberar_carga_previa();

                printf("Memoria liberada correctamente. ¡Hasta luego!\n");
                continuar = 0;
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
                break;
        }
    }
}
