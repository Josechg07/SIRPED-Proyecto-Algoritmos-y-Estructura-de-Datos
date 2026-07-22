#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"
#include "arbol_avl.h"
#include "consultas.h"
#include "lista_historial.h"
#include "menu.h"
#include "memoria.h"
#include "parser.h"
#include "ordenamiento.h"

/*
 * Lee una línea completa de stdin y la interpreta como número de opción.
 * Se usa fgets en vez de scanf("%d", ...) para que TODA la entrada del
 * programa se lea de la misma forma y no haya dos mecanismos de lectura
 * mezclados que puedan desincronizarse entre sí (ese fue el bug que
 * causaba que los campos se corrieran de lugar en versiones anteriores).
 */
static int leer_opcion_menu(void) {
    char buffer[32];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }
    char* fin_num;
    long valor = strtol(buffer, &fin_num, 10);
    if (fin_num == buffer) {
        return -1; /* no había ningún dígito */
    }
    return (int) valor;
}

static void escribir_nodo_archivo(FILE* archivo, NodoAVL* nodo) {
    if (nodo == NULL) return;

    escribir_nodo_archivo(archivo, nodo->izquierdo);

    const NodoHistorial* ultimo = nodo->historial;
    while (ultimo != NULL && ultimo->siguiente != NULL) {
        ultimo = ultimo->siguiente;
    }

    const char* estado = (ultimo != NULL) ? ultimo->estado : "Desaparecido";
    const char* descripcion = (ultimo != NULL) ? ultimo->descripcion : "";

    fprintf(archivo, "%s|%s|%s|%s|%s|%s\n",
            nodo->id_caso,
            nodo->nombre_persona,
            nodo->fecha_inicial,
            nodo->zona_geografica,
            estado,
            descripcion);

    escribir_nodo_archivo(archivo, nodo->derecho);
}

/*
 * NOTA: este archivo guarda solo el ESTADO MÁS RECIENTE de cada caso, no
 * el historial completo (el formato de reportes.txt no tiene espacio
 * para varias entradas por caso). Si vuelves a cargar este mismo archivo
 * con la opción 6, cada caso se recreará con una sola entrada de
 * historial (la última que tenía), no con todo su historial original.
 * Documenta esta limitación en el informe si usas esta función.
 */
static void guardar_arbol_en_archivo(const char* ruta, NodoAVL* raiz) {
    FILE* archivo = fopen(ruta, "w");
    if (archivo == NULL) {
        fprintf(stderr, "[ERROR] No se pudo abrir '%s' para guardar los casos.\n", ruta);
        return;
    }

    escribir_nodo_archivo(archivo, raiz);
    fclose(archivo);
}

int main() {
    NodoAVL* raiz_sistema = NULL;
    int opcion;
    int cantidad_inicial = 0;

    /*
     * Al arrancar, intenta cargar lo que ya estaba guardado en
     * reportes.txt (de una sesión anterior). Si el archivo no existe
     * todavía (primera vez que se corre el programa), usa dos casos de
     * ejemplo y los guarda, para que la próxima vez sí haya algo que
     * cargar. Sin este paso, cada corrida del programa "olvidaba" todo
     * lo guardado en sesiones anteriores.
     */
    ReporteCrudo* reportes_iniciales = cargar_reportes_desde_archivo("reportes.txt", &cantidad_inicial);
    if (reportes_iniciales != NULL && cantidad_inicial > 0) {
        merge_sort_reportes(reportes_iniciales, 0, cantidad_inicial - 1);
        for (int i = 0; i < cantidad_inicial; i++) {
            raiz_sistema = insertar_caso(raiz_sistema,
                reportes_iniciales[i].id_caso, reportes_iniciales[i].nombre_persona,
                reportes_iniciales[i].fecha, reportes_iniciales[i].zona_geografica,
                reportes_iniciales[i].estado, reportes_iniciales[i].descripcion);
        }
        printf("[Info] Se cargaron %d casos guardados previamente desde reportes.txt.\n", cantidad_inicial);
        free(reportes_iniciales);
    } else {
        raiz_sistema = insertar_caso(raiz_sistema, "SIRPED-001", "Pedro Aguila", "2026-06-24", "La Guaira", "Desaparecido", "Reporte Inicial.");
        raiz_sistema = insertar_caso(raiz_sistema, "SIRPED-002", "Maria Espinoza", "2026-07-01", "Caracas", "Desaparecido", "Salio de su residencia.");
        guardar_arbol_en_archivo("reportes.txt", raiz_sistema);
        printf("[Info] No se encontró reportes.txt previo; se creó uno con datos de ejemplo.\n");
    }

    do {
        desplegar_menu();
        opcion = leer_opcion_menu();
        if (opcion == -1) {
            printf("\n[Error] Entrada inválida o fin de la entrada. Cerrando el sistema.\n");
            break;
        }

        if (opcion == 1) {
            char id[20], nombre[100], fecha[16], zona[100], desc[256];
            printf("ID unico (Ej: SIRPED-003): ");
            fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;
            printf("Nombre: ");
            fgets(nombre, sizeof(nombre), stdin); nombre[strcspn(nombre, "\n")] = 0;
            printf("Fecha (AAAA-MM-DD): ");
            fgets(fecha, sizeof(fecha), stdin); fecha[strcspn(fecha, "\n")] = 0;
            printf("Zona: ");
            fgets(zona, sizeof(zona), stdin); zona[strcspn(zona, "\n")] = 0;
            printf("Descripcion: ");
            fgets(desc, sizeof(desc), stdin); desc[strcspn(desc, "\n")] = 0;

            raiz_sistema = insertar_caso(raiz_sistema, id, nombre, fecha, zona, "Desaparecido", desc);
            guardar_arbol_en_archivo("reportes.txt", raiz_sistema);
            printf("\n[Exito] Caso '%s' insertado en AVL y guardado en reportes.txt.\n", id);
            esperar_enter();

        } else if (opcion == 2) {
            char id[20], fecha[16], ubica[100], est[50], desc[256];
            printf("ID del caso a actualizar: ");
            fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;

            NodoAVL* encontrado = buscar_por_id(raiz_sistema, id);
            if (encontrado) {
                printf("Fecha Actualizacion (AAAA-MM-DD): ");
                fgets(fecha, sizeof(fecha), stdin); fecha[strcspn(fecha, "\n")] = 0;
                printf("Ubicacion: ");
                fgets(ubica, sizeof(ubica), stdin); ubica[strcspn(ubica, "\n")] = 0;
                printf("Estado: ");
                fgets(est, sizeof(est), stdin); est[strcspn(est, "\n")] = 0;
                printf("Detalles: ");
                fgets(desc, sizeof(desc), stdin); desc[strcspn(desc, "\n")] = 0;

                agregar_historial(encontrado, fecha, ubica, est, desc);
                guardar_arbol_en_archivo("reportes.txt", raiz_sistema);
                printf("\n[Exito] Historial actualizado y guardado en reportes.txt.\n");
            } else {
                printf("\n[Error] No encontrado.\n");
            }
            esperar_enter();

        } else if (opcion == 3) {
            char inicio[16], fin[16];
            printf("Fecha Inicio (AAAA-MM-DD): ");
            fgets(inicio, sizeof(inicio), stdin); inicio[strcspn(inicio, "\n")] = 0;
            printf("Fecha Fin (AAAA-MM-DD): ");
            fgets(fin, sizeof(fin), stdin); fin[strcspn(fin, "\n")] = 0;
            consultar_rango_fechas(raiz_sistema, inicio, fin);
            esperar_enter();

        } else if (opcion == 4) {
            char zona[100];
            printf("Zona a buscar: ");
            fgets(zona, sizeof(zona), stdin); zona[strcspn(zona, "\n")] = 0;
            consultar_por_zona(raiz_sistema, zona);
            esperar_enter();

        } else if (opcion == 5) {
            char id[20];
            printf("ID del caso: ");
            fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;
            NodoAVL* encontrado = buscar_por_id(raiz_sistema, id);
            if (encontrado) mostrar_caso(encontrado);
            else printf("\nNo encontrado.\n");
            esperar_enter();

        } else if (opcion == 6) {
            char ruta[256];
            int cantidad = 0;

            printf("Ruta del archivo .txt a cargar (ej. reportes.txt): ");
            fgets(ruta, sizeof(ruta), stdin); ruta[strcspn(ruta, "\n")] = 0;

            ReporteCrudo* reportes = cargar_reportes_desde_archivo(ruta, &cantidad);
            if (reportes == NULL || cantidad == 0) {
                printf("\n[Error] No se cargó ningún reporte válido.\n");
            } else {
                printf("Se leyeron %d reportes desde el archivo.\n", cantidad);

                /* Ordenamiento cronológico con Merge Sort antes de insertar */
                merge_sort_reportes(reportes, 0, cantidad - 1);
                printf("Reportes ordenados cronológicamente (Merge Sort).\n");

                for (int i = 0; i < cantidad; i++) {
                    raiz_sistema = insertar_caso(raiz_sistema,
                        reportes[i].id_caso, reportes[i].nombre_persona,
                        reportes[i].fecha, reportes[i].zona_geografica,
                        reportes[i].estado, reportes[i].descripcion);
                }
                printf("[Exito] %d casos insertados en el árbol.\n", cantidad);

                free(reportes); /* arreglo temporal, ya insertado en el árbol */
            }
            esperar_enter();

        } else if (opcion != 7) {
            printf("\n[Aviso] Opción '%d' no reconocida. Elige un número del 1 al 7.\n", opcion);
            esperar_enter();
        }

    } while (opcion != 7);

    printf("\nLiberando recursos dinámicos...\n");
    liberar_sistema(raiz_sistema);
    return 0;
}