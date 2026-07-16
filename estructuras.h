#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#define MAX_NOMBRE 100
#define MAX_UBICACION 100
#define MAX_DESCRIPCION 200

typedef struct NodoHistorial {
    char fecha[11];
    char descripcion[MAX_DESCRIPCION];
    struct NodoHistorial* siguiente;
} NodoHistorial;

typedef struct NodoAVL {
    char nombre[MAX_NOMBRE];
    char ubicacion[MAX_UBICACION];
    char fecha_reporte[11];
    int altura;
    NodoHistorial* historial;
    struct NodoAVL* izquierda;
    struct NodoAVL* derecha;
} NodoAVL;

NodoHistorial* crear_nodo_historial(const char* fecha, const char* descripcion);
NodoHistorial* insertar_historial(NodoHistorial* cabeza, NodoHistorial* nuevo);
void imprimir_historial(NodoHistorial* cabeza);
void liberar_historial(NodoHistorial* cabeza);

NodoAVL* insertar_avl(NodoAVL* raiz, NodoAVL* nuevo);
NodoAVL* buscar_por_rango(NodoAVL* raiz, const char* fecha_inicio, const char* fecha_fin);
void liberar_avl(NodoAVL* raiz);

void merge_sort(NodoAVL** registros, int n);
void ejecutar_menu(NodoAVL* raiz);

#endif