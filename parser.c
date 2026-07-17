/* ============================================================================
 * parser.c
 * ----------------------------------------------------------------------------
 * Se encarga de leer un archivo de texto plano con reportes de personas
 * desaparecidas y volcarlos en un ARREGLO DINÁMICO de structs Reporte.
 *
 * FORMATO DEL ARCHIVO (definido por este módulo):
 *   Cada línea representa un reporte, con campos separados por el
 *   caracter pipe '|' (se eligió '|' en vez de ',' porque la descripción
 *   puede contener comas dentro del texto libre):
 *
 *   id|nombre|dd/mm/aaaa|ubicacion|estado|descripcion
 *
 *   Ejemplo de línea:
 *   1|Juan Perez|15/03/2024|Ciudad de Mexico|Buscando|Visto por ultima vez en el parque
 *
 * Líneas vacías o que comienzan con '#' se ignoran (permite comentarios
 * en el archivo de prueba).
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"

/* Capacidad inicial del arreglo dinámico. Cuando se llena, se duplica
 * mediante realloc (estrategia de crecimiento geométrico, amortiza el
 * costo de las reservas de memoria a O(1) por inserción en promedio). */
#define CAPACIDAD_INICIAL 8

/* ----------------------------------------------------------------------
 * quitar_salto_de_linea
 * Elimina el '\n' (y '\r' en caso de archivos con formato Windows) que
 * fgets() deja al final de la línea leída, modificando el string en
 * el propio arreglo (no reserva memoria nueva).
 * ---------------------------------------------------------------------- */
static void quitar_salto_de_linea(char *linea) {
    size_t len = strlen(linea);
    while (len > 0 && (linea[len - 1] == '\n' || linea[len - 1] == '\r')) {
        linea[len - 1] = '\0';
        len--;
    }
}

/* ----------------------------------------------------------------------
 * parsear_fecha
 * Convierte un texto "dd/mm/aaaa" en una struct Fecha.
 * Retorna 1 si el parseo fue exitoso, 0 si el formato es inválido.
 * ---------------------------------------------------------------------- */
static int parsear_fecha(const char *texto, Fecha *fecha_salida) {
    int dia, mes, anio;
    if (sscanf(texto, "%d/%d/%d", &dia, &mes, &anio) != 3) {
        return 0;
    }
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || anio < 1900) {
        return 0;
    }
    fecha_salida->dia = dia;
    fecha_salida->mes = mes;
    fecha_salida->anio = anio;
    return 1;
}

/* ----------------------------------------------------------------------
 * copiar_campo_seguro
 * Copia un campo de texto (obtenido con strtok) hacia un arreglo de
 * caracteres de tamaño fijo, garantizando que siempre quede terminado
 * en '\0' y sin desbordar el buffer destino (evita overflow, aunque no
 * es "fuga de memoria" en sentido estricto, sí es una buena práctica de
 * robustez que Valgrind también puede detectar como acceso inválido).
 * ---------------------------------------------------------------------- */
static void copiar_campo_seguro(char *destino, const char *origen, size_t tam_destino) {
    if (origen == NULL) {
        destino[0] = '\0';
        return;
    }
    strncpy(destino, origen, tam_destino - 1);
    destino[tam_destino - 1] = '\0';
}

/* ----------------------------------------------------------------------
 * parsear_linea
 * Toma una línea cruda del archivo y la separa en los 6 campos del
 * formato definido, llenando la struct Reporte correspondiente.
 * Retorna 1 si la línea fue válida y se pudo parsear completamente,
 * 0 si la línea debe descartarse (formato incorrecto).
 *
 * Nota sobre memoria: strtok() NO reserva memoria nueva; devuelve
 * punteros que apuntan DENTRO del mismo buffer "linea" que se le pasó,
 * modificándolo (inserta '\0' donde estaban los '|'). Por eso "linea"
 * debe ser un buffer modificable (arreglo local, no un literal de texto).
 * ---------------------------------------------------------------------- */
static int parsear_linea(char *linea, Reporte *r) {
    char *token;
    char texto_fecha[32];

    /* Campo 1: id */
    token = strtok(linea, "|");
    if (token == NULL) return 0;
    r->id = atoi(token);

    /* Campo 2: nombre */
    token = strtok(NULL, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(r->nombre, token, MAX_NOMBRE);

    /* Campo 3: fecha (dd/mm/aaaa) */
    token = strtok(NULL, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(texto_fecha, token, sizeof(texto_fecha));
    if (!parsear_fecha(texto_fecha, &r->fecha_reporte)) {
        return 0;
    }

    /* Campo 4: ubicación */
    token = strtok(NULL, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(r->ubicacion, token, MAX_UBICACION);

    /* Campo 5: estado */
    token = strtok(NULL, "|");
    if (token == NULL) return 0;
    copiar_campo_seguro(r->estado, token, MAX_ESTADO);

    /* Campo 6: descripción (puede contener el resto de la línea) */
    token = strtok(NULL, "\n");
    copiar_campo_seguro(r->descripcion, token, MAX_DESCRIPCION);

    return 1;
}

/* ----------------------------------------------------------------------
 * cargar_reportes_desde_archivo
 * Abre el archivo indicado, lee línea por línea y construye un arreglo
 * DINÁMICO de Reporte usando malloc + realloc.
 *
 * Parámetros de salida:
 *   - cantidad_leida:   número de reportes válidos cargados.
 *   - capacidad_actual: capacidad real reservada en memoria (>= cantidad
 *                        leída). Se expone porque el llamador (menu.c)
 *                        es quien finalmente libera este arreglo con
 *                        liberar_arreglo_reportes(), y es buena práctica
 *                        que quien reserva o quien administra el ciclo de
 *                        vida tenga visibilidad de cuánto se reservó.
 *
 * Retorna: puntero al arreglo dinámico (dueño: el llamador, quien debe
 * liberarlo con liberar_arreglo_reportes()), o NULL si el archivo no
 * pudo abrirse o no contenía reportes válidos.
 * ---------------------------------------------------------------------- */
Reporte *cargar_reportes_desde_archivo(const char *ruta_archivo,
                                        int *cantidad_leida,
                                        int *capacidad_actual) {
    FILE *archivo;
    char linea[MAX_LINEA];
    char linea_original[MAX_LINEA]; /* copia para poder reportar errores */
    int cantidad = 0;
    int capacidad = CAPACIDAD_INICIAL;
    int numero_de_linea = 0;
    Reporte *arreglo;

    *cantidad_leida = 0;
    *capacidad_actual = 0;

    archivo = fopen(ruta_archivo, "r");
    if (archivo == NULL) {
        fprintf(stderr, "Error: no se pudo abrir el archivo '%s'.\n", ruta_archivo);
        return NULL;
    }

    /* Reserva inicial del arreglo dinámico */
    arreglo = (Reporte *) malloc((size_t) capacidad * sizeof(Reporte));
    if (arreglo == NULL) {
        fprintf(stderr, "Error fatal: no se pudo reservar memoria inicial.\n");
        fclose(archivo);
        return NULL;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numero_de_linea++;

        quitar_salto_de_linea(linea);

        /* Ignorar líneas vacías o comentarios (empiezan con '#') */
        if (linea[0] == '\0' || linea[0] == '#') {
            continue;
        }

        /* Guardamos una copia porque strtok() destruye el contenido
         * original de "linea" (inserta '\0' en los separadores) */
        strncpy(linea_original, linea, sizeof(linea_original) - 1);
        linea_original[sizeof(linea_original) - 1] = '\0';

        /* Si el arreglo está lleno, duplicamos su capacidad con realloc.
         * Se usa un puntero temporal "tmp" para no perder la referencia
         * original de "arreglo" si realloc llegara a fallar (retornando
         * NULL): así evitamos una fuga de memoria del bloque anterior. */
        if (cantidad == capacidad) {
            int nueva_capacidad = capacidad * 2;
            Reporte *tmp = (Reporte *) realloc(arreglo, (size_t) nueva_capacidad * sizeof(Reporte));
            if (tmp == NULL) {
                fprintf(stderr, "Error fatal: no se pudo ampliar la memoria (realloc).\n");
                free(arreglo);
                arreglo = NULL;
                fclose(archivo);
                return NULL;
            }
            arreglo = tmp;
            capacidad = nueva_capacidad;
        }

        if (parsear_linea(linea, &arreglo[cantidad])) {
            cantidad++;
        } else {
            fprintf(stderr, "Aviso: línea %d con formato inválido, se omite -> \"%s\"\n",
                    numero_de_linea, linea_original);
        }
    }

    fclose(archivo);

    if (cantidad == 0) {
        fprintf(stderr, "Aviso: el archivo no contenía reportes válidos.\n");
        free(arreglo);
        arreglo = NULL;
        return NULL;
    }

    *cantidad_leida = cantidad;
    *capacidad_actual = capacidad;
    return arreglo;
}
