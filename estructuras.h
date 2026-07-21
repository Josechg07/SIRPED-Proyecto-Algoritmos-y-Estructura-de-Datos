#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

/* ------------------------------------------------------------------------
 * Nodo del historial cronológico de estados de un caso.
 * Cada actualización se agrega al FINAL de la lista, sin borrar ni
 * modificar las entradas anteriores (requisito central del sistema).
 * ------------------------------------------------------------------------ */
typedef struct NodoHistorial {
    char fecha_actualizacion[11]; /* AAAA-MM-DD */
    char ubicacion[100];
    char estado[50];              /* Ej: "Desaparecido", "Avistado", "Localizado" */
    char descripcion[256];
    struct NodoHistorial* siguiente;
} NodoHistorial;

/* ------------------------------------------------------------------------
 * Nodo del Índice Principal (Árbol AVL).
 * Clave de ordenamiento: id_caso (identificador único), NO la fecha.
 * Esto hace que la operación más usada del sistema -registrar/actualizar/
 * buscar un caso por su ID- sea O(log n). Las consultas por rango de
 * fechas y por zona son secundarias y recorren el árbol completo (O(n)),
 * ya que esos campos no son la clave de indexación.
 * ------------------------------------------------------------------------ */
typedef struct NodoAVL {
    char id_caso[20];             /* Clave de búsqueda del árbol */
    char nombre_persona[100];
    char fecha_inicial[11];       /* AAAA-MM-DD, fecha del primer reporte */
    char zona_geografica[100];
    int altura;
    NodoHistorial* historial;     /* Historial de estados de este caso */
    struct NodoAVL* izquierdo;
    struct NodoAVL* derecho;
} NodoAVL;

/* ------------------------------------------------------------------------
 * ReporteCrudo: representa una línea leída desde un archivo .txt, ANTES
 * de insertarse en el árbol. Se usa como elemento de un arreglo dinámico
 * para poder ordenarlo cronológicamente (Merge Sort) antes de insertar
 * los casos uno a uno, tal como exige la propuesta del proyecto.
 * ------------------------------------------------------------------------ */
typedef struct {
    char id_caso[20];
    char nombre_persona[100];
    char fecha[11];                /* AAAA-MM-DD */
    char zona_geografica[100];
    char estado[50];
    char descripcion[256];
} ReporteCrudo;

#endif
