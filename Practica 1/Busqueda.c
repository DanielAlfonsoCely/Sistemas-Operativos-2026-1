#include "Hash.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct Movie buscar_por_nombre(char *nombre , FILE *archivo_bin) {

    
    if (archivo_bin == NULL) {
        perror("Error al abrir el archivo binario");
        struct Movie vacia;
        vacia.primaryTitle[0] = '\0'; // Retornar una estructura vacía si no se puede abrir el archivo
        return vacia;
    }

    unsigned int indice = calcular_hash(nombre);
    long offset = hash_table[indice];

    while (offset != -1) {
        fseek(archivo_bin, offset, SEEK_SET);
        struct Movie pelicula;
        fread(&pelicula, sizeof(struct Movie), 1, archivo_bin);

        if (strcmp(pelicula.primaryTitle, nombre) == 0) {
            return pelicula; // Retorna la película encontrada
        }

        offset = pelicula.next_offset; // Avanza al siguiente offset en la cadena
    }

    struct Movie vacia ;
    vacia.primaryTitle[0] = '\0'; // Retornar una estructura vacía si no se encuentra la película
    return vacia;
}

struct Movie buscar_por_filtros(struct Movie filtros, FILE *archivo_bin){
    //Si un string es = 'N' se ignora ese filtro, si un entero es -1 se ignora ese filtro
    //Se recorre el archivo segun el hash de la pelicula "filtro"
    if (archivo_bin == NULL) {
        perror("Error al abrir el archivo binario");
        struct Movie vacia;
        vacia.primaryTitle[0] = '\0'; // Retornar una estructura vacía si no se puede abrir el archivo
        return vacia;
    }

   
    struct Movie pelicula;

    unsigned int indice = calcular_hash(filtros.primaryTitle);
    long offset = hash_table[indice];

    while (offset != -1) {
        fseek(archivo_bin, offset, SEEK_SET);
        fread(&pelicula, sizeof(struct Movie), 1, archivo_bin);

        int coincide = 1; // Variable para verificar si la película coincide con los filtros

        // Verificar cada filtro, si el filtro no es 'N' o -1, se compara con el valor de la película, si no coincide se marca como no coincidente
        
        if (filtros.titleType[0] != 'N' && strcmp(pelicula.titleType, filtros.titleType) != 0) {
            coincide = 0;
        }
        if (filtros.primaryTitle[0] != 'N' && strcmp(pelicula.primaryTitle, filtros.primaryTitle) != 0) {
            coincide = 0;
        }
        if (filtros.originalTitle[0] != 'N' && strcmp(pelicula.originalTitle, filtros.originalTitle) != 0) {
            coincide = 0;
        }
        if (filtros.isAdult != -1 && pelicula.isAdult != filtros.isAdult) {
            coincide = 0;
        }
        if (filtros.startYear != -1 && pelicula.startYear != filtros.startYear) {
            coincide = 0;
        }
        if (filtros.runtimeMinutes != -1 && pelicula.runtimeMinutes != filtros.runtimeMinutes) {
            coincide = 0;
        }
        if (filtros.genres[0] != 'N' && strstr(pelicula.genres, filtros.genres) == NULL ) { 
            coincide = 0; // Se verifica si el género de la película contiene el género del filtro, si no coincide se marca como no coincidente
        }

        if (coincide == 1) {
            return pelicula; // Retorna la película que coincide con los filtros
        }

        offset = pelicula.next_offset; // Avanza al siguiente offset en la cadena
    }

    struct Movie vacia ;
    vacia.primaryTitle[0] = '\0'; // Retornar una estructura vacía si no se encuentra ninguna película que coincida con los filtros
    return vacia;
}