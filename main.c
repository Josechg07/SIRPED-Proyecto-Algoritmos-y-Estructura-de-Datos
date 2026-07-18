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

int main() {
    NodoAVL* raiz_sistema = NULL;
    int opcion;

    /* Precarga simulada inicial, útil para probar el sistema de una vez */
    raiz_sistema = insertar_caso(raiz_sistema, "SIRPED-001", "Pedro Aguila", "2026-06-24", "La Guaira", "Desaparecido", "Reporte Inicial.");
    raiz_sistema = insertar_caso(raiz_sistema, "SIRPED-002", "Maria Espinoza", "2026-07-01", "Caracas", "Desaparecido", "Salio de su residencia.");

    do {
        desplegar_menu();
        if (scanf("%d", &opcion) != 1) {
            break;
        }
        getchar(); /* Limpiar búfer */

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
            printf("\n[Exito] Caso insertado en AVL.\n");

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
                printf("\n[Exito] Historial actualizado.\n");
            } else {
                printf("\n[Error] No encontrado.\n");
            }

        } else if (opcion == 3) {
            char inicio[16], fin[16];
            printf("Fecha Inicio (AAAA-MM-DD): ");
            fgets(inicio, sizeof(inicio), stdin); inicio[strcspn(inicio, "\n")] = 0;
            printf("Fecha Fin (AAAA-MM-DD): ");
            fgets(fin, sizeof(fin), stdin); fin[strcspn(fin, "\n")] = 0;
            consultar_rango_fechas(raiz_sistema, inicio, fin);

        } else if (opcion == 4) {
            char zona[100];
            printf("Zona a buscar: ");
            fgets(zona, sizeof(zona), stdin); zona[strcspn(zona, "\n")] = 0;
            consultar_por_zona(raiz_sistema, zona);

        } else if (opcion == 5) {
            char id[20];
            printf("ID del caso: ");
            fgets(id, sizeof(id), stdin); id[strcspn(id, "\n")] = 0;
            NodoAVL* encontrado = buscar_por_id(raiz_sistema, id);
            if (encontrado) mostrar_caso(encontrado);
            else printf("\nNo encontrado.\n");

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
        }

    } while (opcion != 7);

    printf("\nLiberando recursos dinámicos...\n");
    liberar_sistema(raiz_sistema);
    return 0;
}
