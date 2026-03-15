#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Hash.h"

char linea [4096]; // Variable global para almacenar cada línea del archivo TSV




// retorna 1 si leyó bien, 0 si línea malformada o fin de archivo
int leer_linea_tsv(FILE *archivo, struct Movie *pelicula){ 

    
    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        return 0; // Fin de archivo o error
    }

    strtok(linea, "\t"); // Ignorar el primer campo (tconst)

    char *titleType = strtok(NULL, "\t"); //Null por que strtok sigue procesando la misma linea hasta el final de esta
    char *primaryTitle = strtok(NULL, "\t");
    char *originalTitle = strtok(NULL, "\t");
    int isAdultStr = atoi(strtok(NULL, "\t"));

    int startYear = atoi(strtok(NULL, "\t")); // Convertir a entero usando atoi
    if(startYear == 0) { // Si el campo startYear esta vacio, se asigna -1 para indicar que no tiene valor
        startYear = -1;
    }

    strtok(NULL, "\t"); // Ignorar el campo endYear

    int runtimeMinutes = atoi(strtok(NULL, "\t"));
    if(runtimeMinutes == 0) { // Si el campo runtimeMinutes esta vacio, se asigna -1 para indicar que no tiene valor
        runtimeMinutes = -1;
    }

    char *genres = strtok(NULL, "\t");
    if (genres[strlen(genres) - 1] == '\n') { // Eliminar el salto de linea al final del campo genres
        genres[strlen(genres) - 1] = '\0';
    }


    // strncpy: Copiar los valores a la estructura pelicula, asegurando no exceder los límites de los campos : destino , fuente , tamaño máximo a copiar
    strncpy(pelicula->titleType, titleType, 31);
    pelicula->titleType[31] = '\0'; // Asegurar el null terminator
    strncpy(pelicula->primaryTitle, primaryTitle,127);
    pelicula->primaryTitle[127] = '\0';
    strncpy(pelicula->originalTitle, originalTitle,127);
    pelicula->originalTitle[127] = '\0';
    pelicula->isAdult = isAdultStr;
    pelicula->startYear = startYear;
    pelicula->runtimeMinutes = runtimeMinutes;
    strncpy(pelicula->genres, genres, 63);
    pelicula->genres[63] = '\0';
        
    return 1; // Línea leída correctamente

}

// escribe la struct y actualiza el hash
void escribir_pelicula_bin(struct Movie *pelicula, FILE *archivo_bin){

    long offset = ftell(archivo_bin);
        int indice = calcular_hash(pelicula->primaryTitle);

        pelicula->next_offset = hash_table[indice];  // apunta al anterior primero
        hash_table[indice] = offset;                // hash apunta al nuevo

        fwrite(pelicula, sizeof(struct Movie), 1, archivo_bin);

}


// Llamada metodos lectura y escritura usando malloc/free para la struct Movie, para evitar que se borre al salir de la función
void convertir_tsv_a_binario(char *archivo_tsv, char *archivo_bin){
    FILE *file = fopen(archivo_tsv, "r");
    if (file == NULL) {
        perror("Error oabriendo el archivo");
        return;
    }

    FILE *archivo_binario = fopen(archivo_bin, "wb");
    if (archivo_binario == NULL) {
        perror("Error abriendo el archivo binario");
        return;
    }

    fgets(linea, sizeof(linea), file);  // Leer la primera línea (cabecera) y descartarla


    struct Movie *pelicula = malloc(sizeof(struct Movie)); // Reservar memoria para la película a leer

    while (leer_linea_tsv(file, pelicula)) { // Leer cada línea del archivo TSV y procesarla
        escribir_pelicula_bin(pelicula, archivo_binario);
    }

    fclose(file);
    fclose(archivo_binario);
    free(pelicula); // Liberar la memoria reservada para la película
}




void insertar_pelicula_en_binario(struct Movie pelicula, char *archivo_hash_bin){ 

    FILE *archivo_bin = fopen(archivo_hash_bin, "ab"); // Abrir en modo append para agregar al final del archivo
    if (archivo_bin == NULL) {
        perror("Error al abrir el archivo binario para insertar la película");
        return;
    }

    fseek(archivo_bin, 0, SEEK_END); // Mover el puntero al final del archivo para escribir la nueva película
    long offset = ftell(archivo_bin);
    
    // encadenar en el hash
    int indice = calcular_hash(pelicula.primaryTitle);
    pelicula.next_offset = hash_table[indice];
    hash_table[indice] = offset;
    
    // escribir
    fwrite(&pelicula, sizeof(struct Movie), 1, archivo_bin);
    
    // actualizar hash.bin
    guardar_hash_en_binario("hash.bin");

    fclose(archivo_bin);
}