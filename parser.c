/* ============================================================================
 * parser.c
 * ----------------------------------------------------------------------------
 * Lee un archivo de texto plano con reportes y los vuelca en un arreglo
 * dinámico de ReporteCrudo, listo para ordenar (ordenamiento.c) e insertar
 * en el árbol (arbol_avl.c).
 *
 * FORMATO DEL ARCHIVO (campos separados por '|'):
 *   id|nombre|AAAA-MM-DD|zona|estado|descripcion
 *
 * Ejemplo:
 *   SIRPED-001|Pedro Aguila|2026-06-24|La Guaira|Desaparecido|Reporte inicial.
 *
 * Líneas vacías o que empiezan con '#' se ignoran.
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"
#include "parser.h"

#define CAPACIDAD_INICIAL 8
#define MAX_LINEA 512

static void quitar_salto_de_linea(char* linea) {
    size_t len = strlen(linea);
    while (len > 0 && (linea[len - 1] == '\n' || linea[len - 1] == '\r')) {
        linea[len - 1] = '\0';
        len--;
    }
}

static void copiar_campo_seguro(char* destino, const char* origen, size_t tam_destino) {
    if (origen == NULL) {
        destino[0] = '\0';
        return;
    }
    strncpy(destino, origen, tam_destino - 1);
    destino[tam_destino - 1] = '\0';
}

static int parsear_linea(char* linea, ReporteCrudo* r) {
    char* token;

    token = strtok(linea, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(r->id_caso, token, sizeof(r->id_caso));

    token = strtok(NULL, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(r->nombre_persona, token, sizeof(r->nombre_persona));

    token = strtok(NULL, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(r->fecha, token, sizeof(r->fecha));

    token = strtok(NULL, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(r->zona_geografica, token, sizeof(r->zona_geografica));

    token = strtok(NULL, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(r->estado, token, sizeof(r->estado));

    token = strtok(NULL, "\n");
    copiar_campo_seguro(r->descripcion, token, sizeof(r->descripcion));

    return 1;
}

ReporteCrudo* cargar_reportes_desde_archivo(const char* ruta_archivo, int* cantidad_leida) {
    FILE* archivo;
    char linea[MAX_LINEA];
    char linea_original[MAX_LINEA];
    int cantidad = 0;
    int capacidad = CAPACIDAD_INICIAL;
    int numero_de_linea = 0;
    ReporteCrudo* arreglo;

    *cantidad_leida = 0;

    archivo = fopen(ruta_archivo, "r");
    if (archivo == NULL) {
        fprintf(stderr, "[ERROR] No se pudo abrir el archivo '%s'.\n", ruta_archivo);
        return NULL;
    }

    arreglo = (ReporteCrudo*) malloc((size_t) capacidad * sizeof(ReporteCrudo));
    if (arreglo == NULL) {
        fprintf(stderr, "[ERROR] No se pudo reservar memoria inicial.\n");
        fclose(archivo);
        return NULL;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numero_de_linea++;
        quitar_salto_de_linea(linea);

        if (linea[0] == '\0' || linea[0] == '#') {
            continue;
        }

        strncpy(linea_original, linea, sizeof(linea_original) - 1);
        linea_original[sizeof(linea_original) - 1] = '\0';

        if (cantidad == capacidad) {
            int nueva_capacidad = capacidad * 2;
            ReporteCrudo* tmp = (ReporteCrudo*) realloc(arreglo, (size_t) nueva_capacidad * sizeof(ReporteCrudo));
            if (tmp == NULL) {
                fprintf(stderr, "[ERROR] No se pudo ampliar la memoria (realloc).\n");
                free(arreglo);
                fclose(archivo);
                return NULL;
            }
            arreglo = tmp;
            capacidad = nueva_capacidad;
        }

        if (parsear_linea(linea, &arreglo[cantidad])) {
            cantidad++;
        } else {
            fprintf(stderr, "[Aviso] Línea %d con formato inválido, se omite -> \"%s\"\n",
                    numero_de_linea, linea_original);
        }
    }

    fclose(archivo);

    if (cantidad == 0) {
        fprintf(stderr, "[Aviso] El archivo no contenía reportes válidos.\n");
        free(arreglo);
        return NULL;
    }

    *cantidad_leida = cantidad;
    return arreglo;
}
