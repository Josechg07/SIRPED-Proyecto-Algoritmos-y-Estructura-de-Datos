/*
 * ===========================================================================
 *  desaparecido.c
 *  Sistema de Recoleccion de Datos — Desaparecidos Terremoto Venezuela 2026
 * ---------------------------------------------------------------------------
 *  Implementacion de funciones auxiliares para la estructura Desaparecido.
 * ===========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "desaparecido.h"

/* ── Creacion ────────────────────────────────────────────────────────────── */

Desaparecido* crearDesaparecido(int id, const char *nombre, const char *cedula,
                                int edad, char genero,
                                const char *fecha, const char *zona,
                                const char *estado, const char *descripcion)
{
    Desaparecido *d = (Desaparecido *)malloc(sizeof(Desaparecido));
    if (!d) {
        fprintf(stderr, "[ERROR] No se pudo asignar memoria para Desaparecido.\n");
        return NULL;
    }

    d->id   = id;
    d->edad = edad;
    d->genero = genero;

    strncpy(d->nombre,             nombre,      99);  d->nombre[99]  = '\0';
    strncpy(d->cedula,             cedula,      19);  d->cedula[19]  = '\0';
    strncpy(d->fecha_desaparicion, fecha,        10);  d->fecha_desaparicion[10] = '\0';
    strncpy(d->zona,               zona,         63);  d->zona[63]    = '\0';
    strncpy(d->estado_actual,      estado,       19);  d->estado_actual[19] = '\0';
    strncpy(d->descripcion,        descripcion, 255);  d->descripcion[255] = '\0';

    return d;
}

/* ── Impresion ───────────────────────────────────────────────────────────── */

void imprimirDesaparecido(const Desaparecido *d)
{
    if (!d) return;

    printf("    ┌─────────────────────────────────────────────\n");
    printf("    │ ID: %d\n",            d->id);
    printf("    │ Nombre:  %s\n",       d->nombre);
    printf("    │ Cedula:  %s\n",       d->cedula);
    printf("    │ Edad:    %d\n",       d->edad);
    printf("    │ Genero:  %c\n",       d->genero);
    printf("    │ Fecha:   %s\n",       d->fecha_desaparicion);
    printf("    │ Zona:    %s\n",       d->zona);
    printf("    │ Estado:  %s\n",       d->estado_actual);
    printf("    │ Desc:    %s\n",       d->descripcion);
    printf("    └─────────────────────────────────────────────\n");
}

/* ── Liberacion ──────────────────────────────────────────────────────────── */

void liberarDesaparecido(Desaparecido *d)
{
    free(d);
}
