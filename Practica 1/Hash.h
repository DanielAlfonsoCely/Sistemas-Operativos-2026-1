#ifndef HASH_H
#define HASH_H

#define HASH_SIZE 999  //Cantidad de espacio que tendra el array del hash

 struct Nodo {
    long offset;
    struct Nodo *siguiente;
    char nombre[128];

};  // Nodo para manejar colisiones, se nombra 2 veces por referenciarse a si mismo



extern struct Nodo *hash_table[HASH_SIZE];

unsigned int calcular_hash(const char *str);

void insertar(long offset, char *nombre);

#endif 

