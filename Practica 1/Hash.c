#include "Hash.h"
#include <string.h>
#include <stdlib.h>

struct Nodo *hash_table[HASH_SIZE]; 

unsigned int calcular_hash(const char *str){
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

void insertar_ordenado(long offset, char *nombre){
    int index = calcular_hash(nombre);
    struct Nodo *nuevo = malloc(sizeof(struct Nodo));
    nuevo->offset = offset;
    strcpy(nuevo->nombre, nombre);
    struct Nodo *actual = hash_table[index];

    if(hash_table[index] == NULL) { // Si el índice esta vacio, se inserta el nuevo nodo
        nuevo -> siguiente = NULL;
        hash_table[index] = nuevo;
    }// Si el nuevo nodo es menor que el nodo actual, se inserta al inicio de la lista
    else if (strcmp(nuevo->nombre, actual->nombre) < 0) {
        nuevo -> siguiente = actual;
        hash_table[index] = nuevo;
    } 
    else { // Si el nuevo nodo es mayor que el nodo actual, se recorre la lista hasta encontrar la posición correcta para insertar el nuevo nodo
        while ( actual->siguiente != NULL &&  strcmp(nuevo->nombre, actual->siguiente->nombre) > 0)
        {
            actual = actual->siguiente;
        }
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
    }
}



long buscar_nombre(char *nombre){
    int index = calcular_hash(nombre);
    struct Nodo *actual = hash_table[index];
    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) { // Si se encuentra el nombre, se retorna el offset
            return actual->offset;
        }
        actual = actual->siguiente;
    }
    return -1; // No encontrado
}


