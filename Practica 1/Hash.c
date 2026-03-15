#include "Hash.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

long hash_table[HASH_SIZE]; 

void inicializar_hash_table() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_table[i] = -1; // Inicializar cada posición del hash con -1 para indicar que esta vacia
    }
}


unsigned int calcular_hash(char *str){
    int c;
    unsigned long hash = 5381; //Valor aleatorio, se usa ese por propiedades matemáticas(Claude)
    c = *str++;
    //Realizar función Hash que esta distribuida casi equitativamente (Claude)
    while(c != '\0'){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        c = *str++;
    }
    return hash % HASH_SIZE;
}



//archivo para guardar el hash de los offsets en el archivo binario
void guardar_hash_en_binario(char *archivo_bin){
    FILE *archivo = fopen(archivo_bin, "wb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para guardar el hash");
        return;
    }
    fwrite(hash_table, sizeof(long), HASH_SIZE, archivo);
    fclose(archivo);
}

//archivo para cargar el hash del archivo binario a memoria ram
void cargar_hash_desde_binario(char *archivo_bin){
    FILE *archivo = fopen(archivo_bin, "rb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para cargar el hash");
        return;
    }
    fread(hash_table, sizeof(long), HASH_SIZE, archivo);
    fclose(archivo);
}

// Función para imprimir los detalles de una película
void imprimir_pelicula(struct Movie pelicula) {
    printf("Title Type: %s\n", pelicula.titleType);
    printf("Primary Title: %s\n", pelicula.primaryTitle);
    printf("Original Title: %s\n", pelicula.originalTitle);
    printf("Is Adult: %d\n", pelicula.isAdult);
    printf("Start Year: %d\n", pelicula.startYear);
    printf("Runtime Minutes: %d\n", pelicula.runtimeMinutes);
    printf("Genres: %s\n", pelicula.genres);
}





