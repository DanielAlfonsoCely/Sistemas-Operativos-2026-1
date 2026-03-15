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

// //void insertar_ordenado(long offset, char *nombre){
//     int index = calcular_hash(nombre);
//     struct Nodo *nuevo = malloc(sizeof(struct Nodo)); // Crear nodo con malloc para evitar se borre al salir de la función
//     nuevo->offset = offset;
//     strncpy(nuevo->nombre, nombre, 127); // Copiar el nombre al nodo, no se puede asignar directamente porque es un arreglo de caracteres destino, origen
//     nuevo->nombre[127] = '\0'; // Asegurar que el nombre termine con un caracter nulo
//     struct Nodo *actual = hash_table[index];
//     if(hash_table[index] == NULL) { // Si el índice esta vacio, se inserta el nuevo nodo
//         nuevo -> siguiente = NULL;
//         hash_table[index] = nuevo;
//     }// Si el nuevo nodo es menor que el nodo actual, se inserta al inicio de la lista
//     else if (strcmp(nuevo->nombre, actual->nombre) < 0) {
//         nuevo -> siguiente = actual;
//         hash_table[index] = nuevo;
//     } 
//     else { // Si el nuevo nodo es mayor que el nodo actual, se recorre la lista hasta encontrar la posición correcta para insertar el nuevo nodo
//         while ( actual->siguiente != NULL &&  strcmp(nuevo->nombre, actual->siguiente->nombre) > 0)
//         {
//             actual = actual->siguiente;
//         }
//         nuevo->siguiente = actual->siguiente;
//         actual->siguiente = nuevo;
//     }
// }

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


void cargar_hash_desde_binario(char *archivo_bin){
    FILE *archivo = fopen(archivo_bin, "rb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para cargar el hash");
        return;
    }
    fread(hash_table, sizeof(long), HASH_SIZE, archivo);
    fclose(archivo);
}

void imprimir_pelicula(struct Movie pelicula) {
    printf("Title Type: %s\n", pelicula.titleType);
    printf("Primary Title: %s\n", pelicula.primaryTitle);
    printf("Original Title: %s\n", pelicula.originalTitle);
    printf("Is Adult: %d\n", pelicula.isAdult);
    printf("Start Year: %d\n", pelicula.startYear);
    printf("Runtime Minutes: %d\n", pelicula.runtimeMinutes);
    printf("Genres: %s\n", pelicula.genres);
}


// //long buscar_nombre(char *nombre){
//     int index = calcular_hash(nombre);
//     struct Nodo *actual = hash_table[index];
//     while (actual != NULL) {
//         if (strcmp(actual->nombre, nombre) == 0) { // Si se encuentra el nombre, se retorna el offset
//             return actual->offset;
//         }
//         actual = actual->siguiente;
//     }
//     return -1; // No encontrado
// }



