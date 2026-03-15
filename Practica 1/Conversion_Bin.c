#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Hash.h"

char linea [4096]; // Variable global para almacenar cada línea del archivo TSV

void convertir_tsv_a_binario( char *archivo_tsv,  char *archivo_bin) {
    printf("Iniciando conversion!\n");

    printf("Abriendo archivo TSV: %s\n", archivo_tsv);
    FILE *file = fopen(archivo_tsv, "r");
    if (file == NULL) {
        perror("Error oabriendo el archivo");
        return;
    }

    printf("Abriendo archivo binario: %s\n", archivo_bin);
    FILE *archivo_binario = fopen(archivo_bin, "wb");
    if (archivo_binario == NULL) {
        perror("Error abriendo el archivo binario");
        return;
    }

    fgets(linea, sizeof(linea), file);  // Leer la primera línea (cabecera) y descartarla
    while (fgets(linea, sizeof(linea), file) != NULL) {

        strtok(linea, "\t"); // Ignorar el primer campo (tconst)
        char *titleType = strtok(NULL, "\t");
        char *primaryTitle = strtok(NULL, "\t");
        char *originalTitle = strtok(NULL, "\t");
        int isAdultStr = atoi(strtok(NULL, "\t"));
        int startYear = atoi(strtok(NULL, "\t"));

        

        if(startYear == 0) { // Si el campo startYear esta vacio, se asigna -1 para indicar que no tiene valor
            startYear = -1;
        }

        strtok(NULL, "\t"); // Ignorar el campo endYear
        int runtimeMinutes = atoi(strtok(NULL, "\t"));

        if(runtimeMinutes == 0) { // Si el campo runtimeMinutes esta vacio, se asigna -1 para indicar que no tiene valor
            runtimeMinutes = -1;
        }

        char *genres = strtok(NULL, "\t");

        if (!titleType || !primaryTitle || !originalTitle || !genres) continue; // saltar líneas malformadas

        if (genres[strlen(genres) - 1] == '\n') { // Eliminar el salto de linea al final del campo genres
            genres[strlen(genres) - 1] = '\0';
        }

        

        struct Movie pelicula;
       
        strncpy(pelicula.titleType, titleType, 31);
        pelicula.titleType[31] = '\0';
        strncpy(pelicula.primaryTitle, primaryTitle,127);
        pelicula.primaryTitle[127] = '\0';
        strncpy(pelicula.originalTitle, originalTitle,127);
        pelicula.originalTitle[127] = '\0';
        pelicula.isAdult = isAdultStr;
        pelicula.startYear = startYear;
        pelicula.runtimeMinutes = runtimeMinutes;
        strncpy(pelicula.genres, genres, 63);
        pelicula.genres[63] = '\0';


        long offset = ftell(archivo_binario);
        int indice = calcular_hash(primaryTitle);

        pelicula.next_offset = hash_table[indice];  // apunta al anterior primero
        hash_table[indice] = offset;                // hash apunta al nuevo

        fwrite(&pelicula, sizeof(struct Movie), 1, archivo_binario);

    }

    fclose(file);
    fclose(archivo_binario);
    
}