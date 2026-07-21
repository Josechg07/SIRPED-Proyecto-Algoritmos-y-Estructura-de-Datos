#ifndef PARSER_H
#define PARSER_H

#include "estructuras.h"

/*
 * Lee el archivo .txt indicado y devuelve un arreglo dinámico de
 * ReporteCrudo. El llamador es dueño de esa memoria y debe liberarla con
 * free() cuando termine de usarla (ver main.c, opción "Cargar masivo").
 * Devuelve NULL si el archivo no se pudo abrir o no tenía datos válidos.
 */
ReporteCrudo* cargar_reportes_desde_archivo(const char* ruta_archivo, int* cantidad_leida);

#endif
