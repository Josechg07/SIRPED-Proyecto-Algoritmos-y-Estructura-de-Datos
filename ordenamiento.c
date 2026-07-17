/* ============================================================================
 * ordenamiento.c
 * ----------------------------------------------------------------------------
 * Implementa el ordenamiento cronológico de los reportes cargados desde el
 * archivo .txt, previo a su inserción (simulada) en el Árbol AVL.
 *
 * ALGORITMO ELEGIDO: MERGE SORT (ordenamiento por mezcla)
 * ----------------------------------------------------------------------------
 * ¿Por qué Merge Sort y no Quick Sort para este caso de uso?
 *   - Merge Sort es un algoritmo ESTABLE: si dos reportes tienen exactamente
 *     la misma fecha, conserva el orden relativo en que aparecían en el
 *     archivo original. Para un sistema de registro de personas, esto es
 *     importante porque no queremos "revolver" el orden de llegada de los
 *     reportes del mismo día.
 *   - Su complejidad en el PEOR caso es O(n log n), garantizada siempre,
 *     a diferencia de Quick Sort cuyo peor caso es O(n^2) (por ejemplo,
 *     si el archivo ya viene parcial u ordenado o con muchos duplicados,
 *     dependiendo del pivote elegido).
 *   - Para cargas masivas desde archivo (que es justamente nuestro caso de
 *     uso: "cargas masivas de reportes"), la previsibilidad de Merge Sort
 *     es más valiosa que la velocidad promedio ligeramente mejor de
 *     Quick Sort en memoria (ambos promedian O(n log n)).
 *
 * COMPLEJIDAD (Big-O) DE MERGE SORT:
 *   - Mejor caso:   O(n log n)
 *   - Caso promedio: O(n log n)
 *   - Peor caso:    O(n log n)
 *   - Espacio:      O(n) auxiliar (arreglo temporal para la mezcla)
 *   - Estable:      Sí
 *
 * El algoritmo divide recursivamente el arreglo en mitades (O(log n)
 * niveles de recursión) y en cada nivel realiza una mezcla lineal O(n),
 * de donde resulta la complejidad total O(n log n).
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "estructuras.h"

/* ----------------------------------------------------------------------
 * comparar_fechas
 * Compara dos fechas cronológicamente.
 * Retorna:
 *   < 0  si 'a' es anterior a 'b'
 *   = 0  si son la misma fecha
 *   > 0  si 'a' es posterior a 'b'
 * Se compara primero el año, luego el mes, luego el día (orden de
 * mayor a menor significancia), tal como se compararían fechas reales.
 * ---------------------------------------------------------------------- */
int comparar_fechas(Fecha a, Fecha b) {
    if (a.anio != b.anio) {
        return a.anio - b.anio;
    }
    if (a.mes != b.mes) {
        return a.mes - b.mes;
    }
    return a.dia - b.dia;
}

/* ----------------------------------------------------------------------
 * mezclar
 * Función auxiliar de Merge Sort: combina dos sub-arreglos ya ordenados
 * arr[inicio..medio] y arr[medio+1..fin] en un único bloque ordenado.
 *
 * Gestión de memoria:
 *   - Se reserva un arreglo temporal dinámico ("temp") con malloc, cuyo
 *     tamaño es exactamente el número de elementos a mezclar. Este es el
 *     ÚNICO malloc de todo el algoritmo de ordenamiento, y se libera con
 *     free() antes de salir de la función, sin importar el camino de
 *     ejecución (no hay "return" tempranos que se salten el free).
 *   - Al liberar "temp", el puntero deja de usarse inmediatamente después
 *     (es una variable local que muere al salir de la función), por lo
 *     que no queda como puntero colgante accesible desde el resto del
 *     programa.
 * ---------------------------------------------------------------------- */
static void mezclar(Reporte arr[], int inicio, int medio, int fin) {
    int i, j, k;
    int n_izq = medio - inicio + 1;   /* tamaño del sub-arreglo izquierdo */
    int n_der = fin - medio;          /* tamaño del sub-arreglo derecho   */

    /* Reservamos memoria dinámica para el arreglo temporal de mezcla.
     * Usamos (n_izq + n_der) structs Reporte completos. */
    Reporte *temp = (Reporte *) malloc((size_t)(n_izq + n_der) * sizeof(Reporte));
    if (temp == NULL) {
        fprintf(stderr, "Error fatal: no se pudo reservar memoria en mezclar().\n");
        exit(EXIT_FAILURE);
    }

    /* Copiamos ambos sub-arreglos al arreglo temporal, uno tras otro */
    for (i = 0; i < n_izq; i++) {
        temp[i] = arr[inicio + i];
    }
    for (j = 0; j < n_der; j++) {
        temp[n_izq + j] = arr[medio + 1 + j];
    }

    /* Mezclamos comparando fecha por fecha, tomando siempre el reporte
     * cronológicamente más antiguo de los dos "montones" */
    i = 0;              /* índice para la mitad izquierda dentro de temp */
    j = n_izq;          /* índice para la mitad derecha dentro de temp   */
    k = inicio;         /* índice de escritura en el arreglo original    */

    while (i < n_izq && j < (n_izq + n_der)) {
        if (comparar_fechas(temp[i].fecha_reporte, temp[j].fecha_reporte) <= 0) {
            arr[k] = temp[i];
            i++;
        } else {
            arr[k] = temp[j];
            j++;
        }
        k++;
    }

    /* Copiamos los elementos restantes (si alguno de los dos montones
     * todavía tiene elementos sin volcar al arreglo original) */
    while (i < n_izq) {
        arr[k] = temp[i];
        i++;
        k++;
    }
    while (j < (n_izq + n_der)) {
        arr[k] = temp[j];
        j++;
        k++;
    }

    /* Liberamos el arreglo temporal: ya cumplió su propósito. */
    free(temp);
    temp = NULL; /* buena práctica: evitar puntero colgante, aunque "temp"
                    es una variable local que de todas formas desaparece
                    al salir de la función. */
}

/* ----------------------------------------------------------------------
 * merge_sort_reportes
 * Ordena recursivamente arr[inicio..fin] (ambos índices inclusive) de
 * forma ascendente por fecha de reporte, usando el algoritmo Merge Sort.
 *
 * No reserva memoria directamente (la reserva/liberación ocurre dentro
 * de mezclar()), por lo que esta función no introduce fugas por sí misma.
 * ---------------------------------------------------------------------- */
void merge_sort_reportes(Reporte arr[], int inicio, int fin) {
    if (inicio < fin) {
        int medio = inicio + (fin - inicio) / 2; /* evita overflow vs (inicio+fin)/2 */

        merge_sort_reportes(arr, inicio, medio);
        merge_sort_reportes(arr, medio + 1, fin);

        mezclar(arr, inicio, medio, fin);
    }
}
