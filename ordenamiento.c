/* ============================================================================
 * ordenamiento.c
 * ----------------------------------------------------------------------------
 * Ordena cronológicamente los reportes leídos de un archivo .txt (parser.c)
 * ANTES de insertarlos uno a uno en el árbol AVL, tal como exige la
 * propuesta del proyecto.
 *
 * ALGORITMO: Merge Sort.
 *   - Estable: si dos reportes comparten fecha, conserva el orden en que
 *     aparecían en el archivo original.
 *   - Peor caso garantizado O(n log n), a diferencia de Quick Sort (O(n^2)
 *     en el peor caso, por ejemplo con el archivo ya parcialmente ordenado).
 *   - Complejidad: O(n log n) en todos los casos; espacio O(n) auxiliar.
 * ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"
#include "ordenamiento.h"

/* Fechas en formato AAAA-MM-DD: strcmp preserva el orden cronológico. */
static int comparar_fechas(const char* a, const char* b) {
    return strcmp(a, b);
}

static void mezclar(ReporteCrudo arr[], int inicio, int medio, int fin) {
    int n_izq = medio - inicio + 1;
    int n_der = fin - medio;

    ReporteCrudo* temp = (ReporteCrudo*) malloc((size_t)(n_izq + n_der) * sizeof(ReporteCrudo));
    if (!temp) {
        fprintf(stderr, "[ERROR] No se pudo reservar memoria en mezclar().\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_izq; i++) temp[i] = arr[inicio + i];
    for (int j = 0; j < n_der; j++) temp[n_izq + j] = arr[medio + 1 + j];

    int i = 0, j = n_izq, k = inicio;
    while (i < n_izq && j < (n_izq + n_der)) {
        if (comparar_fechas(temp[i].fecha, temp[j].fecha) <= 0) {
            arr[k++] = temp[i++];
        } else {
            arr[k++] = temp[j++];
        }
    }
    while (i < n_izq) arr[k++] = temp[i++];
    while (j < (n_izq + n_der)) arr[k++] = temp[j++];

    free(temp);
}

void merge_sort_reportes(ReporteCrudo arr[], int inicio, int fin) {
    if (inicio < fin) {
        int medio = inicio + (fin - inicio) / 2;
        merge_sort_reportes(arr, inicio, medio);
        merge_sort_reportes(arr, medio + 1, fin);
        mezclar(arr, inicio, medio, fin);
    }
}
