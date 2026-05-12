#include "imdb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Función para buscar una película por su título principal utilizando la tabla hash
 Movie buscar_por_nombre(char *nombre , FILE *archivo_bin) {

    
    if (archivo_bin == NULL) {
        perror("Error al abrir el archivo binario");
        Movie vacia;
        vacia.primaryTitle[0] = '\0'; // Retornar una estructura vacía si no se puede abrir el archivo
        return vacia;
    }

    // Calcular el índice del hash para el nombre de la película y buscar en la tabla hash
    unsigned int indice = calcular_hash(nombre);
    long offset = hash_table[indice]; 

    // Recorrer la cadena de offsets en la tabla hash para encontrar la película con el título principal dado
    while (offset != -1) {
        fseek(archivo_bin, offset, SEEK_SET); // Mover el puntero del archivo al offset actual, (donde se encuentra la struct de la película)
        Movie pelicula;
        fread(&pelicula, sizeof(Movie), 1, archivo_bin); // Leer la pelicula en la posición actual del archivo, y almacenarla en la variable pelicula

        if (strcmp(pelicula.primaryTitle, nombre) == 0) {
            return pelicula; // Retorna la película encontrada
        }

        offset = pelicula.next_offset; // Avanza al siguiente offset en la cadena
    }

    Movie vacia ;
    vacia.primaryTitle[0] = '\0'; // Retornar una estructura vacía si no se encuentra la película
    return vacia;
}

// Función para buscar una película por múltiples filtros, utilizando la tabla hash para optimizar la búsqueda
Movie buscar_por_filtros(Movie filtros, FILE *archivo_bin){
    //Si un string es = 'N' se ignora ese filtro, si un entero es -1 se ignora ese filtro
    
    if (archivo_bin == NULL) {
        perror("Error al abrir el archivo binario");
        Movie vacia;
        vacia.primaryTitle[0] = '\0'; // Retornar una estructura vacía si no se puede abrir el archivo
        return vacia;
    }

    
    Movie pelicula;

    unsigned int indice = calcular_hash(filtros.primaryTitle);
    long offset = hash_table[indice];

    //Se recorre el archivo segun el hash de la pelicula "filtros"
    while (offset != -1) {
        fseek(archivo_bin, offset, SEEK_SET); // Mover el puntero del archivo al offset actual, (donde se encuentra la struct de la película)
        fread(&pelicula, sizeof(Movie), 1, archivo_bin);// Leer la pelicula en la posición actual del archivo, y almacenarla en la variable pelicula

        int coincide = 1; // Variable para verificar si la película coincide con los filtros

        // Verificar cada filtro, si el filtro no es '\0' o -1, se compara con el valor de la película, si no coincide se marca como no coincidente
        
        if (filtros.titleType[0] != '\0' && strcmp(pelicula.titleType, filtros.titleType) != 0) { 
            coincide = 0;
        }
        if (filtros.primaryTitle[0] != '\0' && strcmp(pelicula.primaryTitle, filtros.primaryTitle) != 0) {
            coincide = 0;
        }
        if (filtros.originalTitle[0] != '\0' && strcmp(pelicula.originalTitle, filtros.originalTitle) != 0) {// Si el filtro de originalTitle no es '\0' y no coincide con el originalTitle de la película, se marca como no coincidente
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
        if (filtros.genres[0] != '\0' && strstr(pelicula.genres, filtros.genres) == NULL ) { // Se verifica si el género de la película contiene el género del filtro, si no coincide se marca como no coincidente
            coincide = 0; 
        }

        if (coincide == 1) {
            return pelicula; // Retorna la película que coincide con los filtros
        }

        offset = pelicula.next_offset; // Avanza al siguiente offset en la cadena
    }

    Movie vacia ;
    vacia.primaryTitle[0] = '\0'; // Retornar una estructura vacía si no se encuentra ninguna película que coincida con los filtros
    return vacia;
}