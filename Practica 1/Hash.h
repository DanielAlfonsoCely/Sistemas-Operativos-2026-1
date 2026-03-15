#ifndef HASH_H
#define HASH_H
#include <stdio.h>
#define HASH_SIZE 999  //Cantidad de espacio que tendra el array del hash



struct Movie {
    char titleType[32];
    char primaryTitle[128];
    char originalTitle[128];
    int isAdult;
    int  startYear;
    int runtimeMinutes;
    char genres[64];
    long next_offset;
};

void imprimir_pelicula(struct Movie pelicula);

extern long hash_table[HASH_SIZE];

unsigned int calcular_hash( char *str);
void inicializar_hash_table();



//void insertar_ordenado(long offset, char *nombre);
void convertir_tsv_a_binario( char *archivo_tsv,  char *archivo_bin);
//long buscar_nombre(char *nombre);
void guardar_hash_en_binario( char *archivo_bin);
void cargar_hash_desde_binario( char *archivo_bin);

struct Movie buscar_por_nombre(char *nombre , FILE *archivo_bin);
struct Movie buscar_por_filtros(struct Movie filtros, FILE *archivo_bin);

#endif 

