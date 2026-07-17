/* ============================================================================
 * memoria.c
 * ----------------------------------------------------------------------------
 * Contiene:
 *   1) Las funciones MOCK que simulan la inserción en el Árbol AVL y en la
 *      Lista Enlazada de historial (a reemplazar por el equipo de AVL/Listas).
 *   2) Las funciones de LIBERACIÓN de toda la memoria dinámica reservada
 *      por el sistema: arreglo de ordenamiento, árbol simulado y listas
 *      de historial colgadas de cada nodo.
 *
 * Regla general seguida en todo el archivo:
 *   Después de cada free(puntero), se asigna puntero = NULL. Así, si por
 *   error alguna otra parte del código intentara usar ese puntero de
 *   nuevo, el programa fallaría de forma segura y detectable (NULL
 *   dereference) en vez de corromper memoria silenciosamente (dangling
 *   pointer / use-after-free), que es mucho más difícil de depurar y es
 *   justo lo que Valgrind reporta como "Invalid read/write".
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"

/* ========================================================================
 * SECCIÓN 1: LISTA DE HISTORIAL (mock, equipo de Listas la reemplazará)
 * ======================================================================== */

/* ----------------------------------------------------------------------
 * crear_lista_historial
 * Reserva y devuelve una ListaHistorial vacía (cabeza == NULL).
 * El llamador es dueño de esta memoria y debe liberarla eventualmente
 * con liberar_lista_historial().
 * ---------------------------------------------------------------------- */
ListaHistorial *crear_lista_historial(void) {
    ListaHistorial *lista = (ListaHistorial *) malloc(sizeof(ListaHistorial));
    if (lista == NULL) {
        fprintf(stderr, "Error fatal: no se pudo reservar memoria para la lista de historial.\n");
        exit(EXIT_FAILURE);
    }
    lista->cabeza = NULL;
    return lista;
}

/* ----------------------------------------------------------------------
 * insertar_nodo_historial
 * Inserta un nuevo estado AL FRENTE de la lista (inserción por cabeza,
 * O(1)), preservando todo el historial anterior tal como exige el
 * diseño del sistema ("no sobrescribir información anterior").
 * ---------------------------------------------------------------------- */
void insertar_nodo_historial(ListaHistorial *lista, const char *estado,
                              Fecha fecha, const char *descripcion) {
    NodoHistorial *nuevo;

    if (lista == NULL) {
        return; /* defensivo: no hay lista donde insertar */
    }

    nuevo = (NodoHistorial *) malloc(sizeof(NodoHistorial));
    if (nuevo == NULL) {
        fprintf(stderr, "Error fatal: no se pudo reservar memoria para un nodo de historial.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(nuevo->estado, estado, MAX_ESTADO - 1);
    nuevo->estado[MAX_ESTADO - 1] = '\0';

    strncpy(nuevo->descripcion, descripcion, MAX_DESCRIPCION - 1);
    nuevo->descripcion[MAX_DESCRIPCION - 1] = '\0';

    nuevo->fecha_cambio = fecha;
    nuevo->siguiente = lista->cabeza;
    lista->cabeza = nuevo;
}

/* ----------------------------------------------------------------------
 * liberar_lista_historial
 * Recorre la lista de forma ITERATIVA liberando nodo por nodo, y al
 * final libera también la struct contenedora ListaHistorial.
 * Recibe un DOBLE PUNTERO (ListaHistorial **) para poder poner en NULL
 * el puntero del LLAMADOR una vez liberada la memoria (si solo
 * recibiéramos ListaHistorial *, el NULL solo aplicaría a la copia
 * local del puntero dentro de esta función, no al puntero original que
 * vive en el nodo del árbol).
 * ---------------------------------------------------------------------- */
void liberar_lista_historial(ListaHistorial **lista) {
    NodoHistorial *actual;
    NodoHistorial *siguiente;

    if (lista == NULL || *lista == NULL) {
        return;
    }

    actual = (*lista)->cabeza;
    while (actual != NULL) {
        siguiente = actual->siguiente; /* guardamos referencia antes de liberar */
        free(actual);
        actual = siguiente;
    }

    free(*lista);
    *lista = NULL; /* el puntero del llamador queda apuntando a NULL */
}

/* ========================================================================
 * SECCIÓN 2: ÁRBOL AVL (mock, equipo de AVL lo reemplazará con rotaciones
 * de balanceo; aquí se implementa como un Árbol Binario de Búsqueda
 * simple ordenado por fecha de reporte, únicamente para poder probar
 * el resto del sistema de forma end-to-end)
 * ======================================================================== */

/* ----------------------------------------------------------------------
 * mock_insertar_avl
 * Inserta un nuevo Reporte en el árbol simulado, ordenado por fecha de
 * reporte (y por id como criterio de desempate si la fecha coincide).
 * Crea también la ListaHistorial vacía del nodo y le agrega un primer
 * registro de historial con el estado inicial del reporte.
 *
 * NOTA PARA INTEGRACIÓN FUTURA: esta función es exactamente el punto
 * que el equipo de AVL debe reemplazar por su "avl_insertar()" real
 * (que además reequilibra el árbol). La firma (Reporte entra, NodoAVL*
 * sale) puede mantenerse igual para no tener que tocar menu.c ni main.c.
 * ---------------------------------------------------------------------- */
NodoAVL *mock_insertar_avl(NodoAVL *raiz, Reporte r) {
    if (raiz == NULL) {
        NodoAVL *nuevo = (NodoAVL *) malloc(sizeof(NodoAVL));
        if (nuevo == NULL) {
            fprintf(stderr, "Error fatal: no se pudo reservar memoria para un nodo del arbol.\n");
            exit(EXIT_FAILURE);
        }
        nuevo->dato = r;
        nuevo->izquierdo = NULL;
        nuevo->derecho = NULL;
        nuevo->altura = 1;

        /* Cada nodo nace con su propia lista de historial, iniciada con
         * el estado con el que llega el reporte por primera vez. */
        nuevo->historial = crear_lista_historial();
        insertar_nodo_historial(nuevo->historial, r.estado, r.fecha_reporte,
                                 "Registro inicial del reporte.");

        return nuevo;
    }

    if (comparar_fechas(r.fecha_reporte, raiz->dato.fecha_reporte) < 0) {
        raiz->izquierdo = mock_insertar_avl(raiz->izquierdo, r);
    } else if (comparar_fechas(r.fecha_reporte, raiz->dato.fecha_reporte) > 0) {
        raiz->derecho = mock_insertar_avl(raiz->derecho, r);
    } else {
        /* Misma fecha: desempatamos por id para mantener un orden total */
        if (r.id < raiz->dato.id) {
            raiz->izquierdo = mock_insertar_avl(raiz->izquierdo, r);
        } else {
            raiz->derecho = mock_insertar_avl(raiz->derecho, r);
        }
    }

    return raiz;
}

/* ----------------------------------------------------------------------
 * buscar_por_id
 * Recorre el árbol (recursivamente, ambos subárboles si es necesario,
 * ya que el árbol está ordenado por FECHA y no por id) buscando el nodo
 * cuyo Reporte.id coincide con el buscado.
 * Retorna el puntero al nodo encontrado, o NULL si no existe.
 * No reserva ni libera memoria: solo consulta.
 * ---------------------------------------------------------------------- */
NodoAVL *buscar_por_id(NodoAVL *raiz, int id) {
    NodoAVL *encontrado;

    if (raiz == NULL) {
        return NULL;
    }
    if (raiz->dato.id == id) {
        return raiz;
    }

    encontrado = buscar_por_id(raiz->izquierdo, id);
    if (encontrado != NULL) {
        return encontrado;
    }
    return buscar_por_id(raiz->derecho, id);
}

/* ----------------------------------------------------------------------
 * mostrar_reportes_inorder
 * Recorrido InOrder del árbol: como está ordenado por fecha, este
 * recorrido imprime los reportes en orden CRONOLÓGICO ascendente.
 * Solo lectura: no reserva ni libera memoria.
 * ---------------------------------------------------------------------- */
void mostrar_reportes_inorder(const NodoAVL *raiz) {
    if (raiz == NULL) {
        return;
    }
    mostrar_reportes_inorder(raiz->izquierdo);

    printf("  [ID %-4d] %-20s | %02d/%02d/%04d | %-15s | Estado: %s\n",
           raiz->dato.id, raiz->dato.nombre,
           raiz->dato.fecha_reporte.dia, raiz->dato.fecha_reporte.mes,
           raiz->dato.fecha_reporte.anio, raiz->dato.ubicacion,
           raiz->dato.estado);

    mostrar_reportes_inorder(raiz->derecho);
}

/* ----------------------------------------------------------------------
 * contar_nodos_avl
 * Cuenta cuántos nodos (casos) hay actualmente en el árbol. Útil para
 * mostrar estadísticas en el menú.
 * ---------------------------------------------------------------------- */
int contar_nodos_avl(const NodoAVL *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    return 1 + contar_nodos_avl(raiz->izquierdo) + contar_nodos_avl(raiz->derecho);
}

/* ----------------------------------------------------------------------
 * liberar_arbol_avl
 * Libera TODO el árbol de forma RECURSIVA usando un recorrido POSTORDER
 * (primero se liberan los hijos, después el nodo actual). Esto es
 * obligatorio: si liberáramos el nodo actual antes que sus hijos,
 * perderíamos los punteros necesarios para llegar a esos hijos y
 * causaríamos una fuga de memoria masiva e irreversible.
 *
 * Por cada nodo, antes de hacer free() del nodo mismo, se libera
 * también su ListaHistorial asociada (liberar_lista_historial), ya
 * que esa lista es memoria dinámica "colgada" del nodo y nadie más
 * tiene una referencia a ella.
 *
 * Recibe doble puntero (NodoAVL **) por la misma razón explicada en
 * liberar_lista_historial: para dejar en NULL el puntero raíz que vive
 * en el llamador (por ejemplo, la variable global/local del main o del
 * menú), evitando así un puntero colgante hacia memoria ya liberada.
 * ---------------------------------------------------------------------- */
void liberar_arbol_avl(NodoAVL **raiz) {
    if (raiz == NULL || *raiz == NULL) {
        return;
    }

    /* Postorder: primero izquierda, luego derecha, al final el nodo */
    liberar_arbol_avl(&(*raiz)->izquierdo);
    liberar_arbol_avl(&(*raiz)->derecho);

    /* Liberamos la lista de historial colgada de este nodo */
    liberar_lista_historial(&(*raiz)->historial);

    free(*raiz);
    *raiz = NULL;
}

/* ========================================================================
 * SECCIÓN 3: ARREGLO DINÁMICO DE REPORTES (usado por ordenamiento/parser)
 * ======================================================================== */

/* ----------------------------------------------------------------------
 * liberar_arreglo_reportes
 * Libera el arreglo dinámico de reportes reservado en parser.c
 * (malloc/realloc). Como los campos de Reporte son arreglos de
 * caracteres de tamaño FIJO (no punteros), basta con un único free()
 * sobre el arreglo completo: no hay memoria "anidada" adicional que
 * liberar campo por campo.
 * ---------------------------------------------------------------------- */
void liberar_arreglo_reportes(Reporte **arr) {
    if (arr == NULL || *arr == NULL) {
        return;
    }
    free(*arr);
    *arr = NULL;
}
