/* ============================================================================
 * estructuras.h
 * ----------------------------------------------------------------------------
 * Proyecto: SIRPED (Sistema de Registro y Seguimiento de Personas Desaparecidas)
 * Módulo:   Ordenamiento + Integración (main, menú, memoria)
 * ----------------------------------------------------------------------------
 * Este archivo define:
 *   1) La estructura real "Reporte", que representa un caso/persona.
 *   2) Estructuras MOCK (temporales) que simulan el Árbol AVL y la Lista
 *      Enlazada que están desarrollando mis compañeros de equipo. Estas
 *      estructuras mock respetan la MISMA "forma" conceptual que tendrán
 *      las reales (un nodo de árbol que apunta a una lista de historial),
 *      de modo que cuando el equipo entregue su implementación definitiva,
 *      solo haya que:
 *          - Reemplazar este .h por el .h oficial del árbol/lista, o
 *          - Sustituir las funciones mock (mock_insertar_avl, etc.) por
 *            las funciones reales del AVL (avl_insertar, avl_balancear...).
 *      El resto de mis módulos (ordenamiento, parser, menu, memoria, main)
 *      NO conocen detalles internos del AVL real: solo usan estas funciones
 *      "puente", por lo que el acoplamiento es mínimo (bajo acoplamiento).
 * ============================================================================
 */

#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

/* ----------------------------------------------------------------------
 * Constantes de tamaño para los campos de texto (arreglos de caracteres
 * de tamaño fijo). Se usan arreglos fijos -en vez de punteros char* con
 * malloc individual- para simplificar la gestión de memoria de cada
 * Reporte: al copiar un "Reporte" (struct) se copian también sus cadenas,
 * sin necesidad de duplicar memoria dinámica campo por campo. Esto reduce
 * drásticamente el riesgo de fugas y de punteros colgantes.
 * ---------------------------------------------------------------------- */
#define MAX_NOMBRE       100
#define MAX_UBICACION    100
#define MAX_ESTADO       50
#define MAX_DESCRIPCION  255
#define MAX_LINEA        512   /* longitud máxima de una línea del .txt */

/* ----------------------------------------------------------------------
 * Fecha
 * Se separa en día/mes/año para facilitar tanto la lectura desde el
 * archivo de texto como la comparación cronológica en el ordenamiento.
 * ---------------------------------------------------------------------- */
typedef struct {
    int dia;
    int mes;
    int anio;
} Fecha;

/* ----------------------------------------------------------------------
 * Reporte
 * Estructura principal del sistema: un caso de persona desaparecida.
 * Esta es la estructura "real" y definitiva (no es mock), ya que el
 * formato de los datos fue acordado por todo el equipo.
 * ---------------------------------------------------------------------- */
typedef struct {
    int    id;                          /* identificador único del caso   */
    char   nombre[MAX_NOMBRE];          /* nombre de la persona           */
    Fecha  fecha_reporte;               /* fecha en que se reportó        */
    char   ubicacion[MAX_UBICACION];    /* lugar de la desaparición       */
    char   estado[MAX_ESTADO];          /* estado actual (ej. "Buscando") */
    char   descripcion[MAX_DESCRIPCION];/* detalles adicionales           */
} Reporte;

/* ========================================================================
 * ESTRUCTURAS MOCK (TEMPORALES)
 * Simulan el Árbol AVL y la Lista Enlazada de historial de estados.
 * Serán sustituidas por las estructuras reales del equipo de AVL/Listas.
 * ======================================================================== */

/* Nodo de la lista enlazada de historial de estados de una persona.
 * Cada vez que el estado de un caso cambia (ej. "Buscando" -> "Encontrado"),
 * se agrega un nuevo nodo al FRENTE de la lista, sin borrar el anterior,
 * para no perder el historial. */
typedef struct NodoHistorial {
    char   estado[MAX_ESTADO];
    Fecha  fecha_cambio;
    char   descripcion[MAX_DESCRIPCION];
    struct NodoHistorial *siguiente;   /* puntero al siguiente nodo (o NULL) */
} NodoHistorial;

/* Cabecera de la lista de historial. Se envuelve la cabeza en un struct
 * (en vez de usar un simple NodoHistorial*) para poder pasarla por
 * referencia de forma más clara y para que el equipo de Listas pueda
 * extenderla en el futuro (ej. agregar un contador de nodos) sin romper
 * la firma de las funciones que ya la usan. */
typedef struct {
    NodoHistorial *cabeza;
} ListaHistorial;

/* Nodo del Árbol Binario/AVL "mock". Internamente es un Árbol Binario de
 * Búsqueda simple (sin rotaciones de balanceo) ordenado por fecha de
 * reporte. El campo "altura" ya está presente para que, cuando se integre
 * el AVL real, el equipo solo tenga que agregar la lógica de rotación
 * (rotar_izquierda/rotar_derecha) sin cambiar la forma del nodo.
 * Cada nodo apunta a su propia ListaHistorial (relación 1 a 1 caso-historial). */
typedef struct NodoAVL {
    Reporte dato;                 /* información del caso                */
    ListaHistorial *historial;    /* historial de estados de esta persona*/
    struct NodoAVL *izquierdo;
    struct NodoAVL *derecho;
    int altura;                   /* usado por el AVL real para balancear */
} NodoAVL;

/* ========================================================================
 * PROTOTIPOS DE FUNCIONES
 * ======================================================================== */

/* ---- ordenamiento.c ---- */
void merge_sort_reportes(Reporte arr[], int inicio, int fin);
int  comparar_fechas(Fecha a, Fecha b);

/* ---- parser.c ---- */
Reporte *cargar_reportes_desde_archivo(const char *ruta_archivo,
                                        int *cantidad_leida,
                                        int *capacidad_actual);

/* ---- memoria.c ---- */
void      liberar_arreglo_reportes(Reporte **arr);
NodoAVL  *mock_insertar_avl(NodoAVL *raiz, Reporte r);
void      liberar_arbol_avl(NodoAVL **raiz);
ListaHistorial *crear_lista_historial(void);
void      insertar_nodo_historial(ListaHistorial *lista, const char *estado,
                                   Fecha fecha, const char *descripcion);
void      liberar_lista_historial(ListaHistorial **lista);
NodoAVL  *buscar_por_id(NodoAVL *raiz, int id);
void      mostrar_reportes_inorder(const NodoAVL *raiz);
int       contar_nodos_avl(const NodoAVL *raiz);

/* ---- menu.c ---- */
void ejecutar_menu(void);

#endif /* ESTRUCTURAS_H */
