/*
 * ===========================================================================
 *  desaparecido.h
 *  Sistema de Recoleccion de Datos — Desaparecidos Terremoto Venezuela 2026
 * ---------------------------------------------------------------------------
 *  Estructura de datos para representar a una persona desaparecida.
 * ===========================================================================
 */

#ifndef DESAPARECIDO_H
#define DESAPARECIDO_H

#include <stdio.h>

/* ── Estructura principal ────────────────────────────────────────────────── */

typedef struct Desaparecido {
    int   id;                       /* Identificador unico auto-incremental */
    char  nombre[100];              /* Nombre completo                      */
    char  cedula[20];               /* Cedula de identidad                  */
    int   edad;                     /* Edad al momento de la desaparicion   */
    char  genero;                   /* 'M' / 'F' / 'O'                     */
    char  fecha_desaparicion[11];   /* Formato YYYY-MM-DD                   */
    char  zona[64];                 /* Zona geografica de desaparicion      */
    char  estado_actual[20];        /* DESAPARECIDO, ENCONTRADO, FALLECIDO  */
    char  descripcion[256];         /* Descripcion fisica / info adicional  */
} Desaparecido;

/* ── Funciones auxiliares ────────────────────────────────────────────────── */

/*
 * Crea un nuevo registro de persona desaparecida en memoria dinamica.
 * Retorna un puntero al registro creado, o NULL si falla la asignacion.
 */
Desaparecido* crearDesaparecido(int id, const char *nombre, const char *cedula,
                                int edad, char genero,
                                const char *fecha, const char *zona,
                                const char *estado, const char *descripcion);

/*
 * Imprime los datos de una persona desaparecida en stdout.
 */
void imprimirDesaparecido(const Desaparecido *d);

/*
 * Libera la memoria de un registro de persona desaparecida.
 */
void liberarDesaparecido(Desaparecido *d);

#endif /* DESAPARECIDO_H */
