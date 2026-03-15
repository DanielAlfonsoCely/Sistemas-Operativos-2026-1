#ifndef HASH_H
#define HASH_H
#include <stdio.h>
#define HASH_SIZE 999  //Cantidad de espacio que tendra el array del hash


// Estructura para almacenar la información de una película, con un campo adicional para el offset del siguiente elemento en caso de colisión
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

// Declaración de la tabla hash como un array global de long, donde cada posición almacena el offset del primer elemento en la cadena de colisiones
extern long hash_table[HASH_SIZE];


// Función para imprimir los detalles de una película
void imprimir_pelicula(struct Movie pelicula);


// Función para calcular el hash de una cadena de caracteres 
unsigned int calcular_hash( char *str);
//Inicializar la tabla hash con -1 para indicar que cada posición esta vacia
void inicializar_hash_table();


int leer_linea_tsv(FILE *archivo, struct Movie *pelicula);
// retorna 1 si leyó bien, 0 si línea malformada o fin de archivo

void escribir_pelicula_bin(struct Movie *pelicula, FILE *archivo_bin);
// escribe la struct y actualiza el hash

void convertir_tsv_a_binario(char *archivo_tsv, char *archivo_bin);
// el loop con malloc/free

void insertar_pelicula_en_binario(struct Movie pelicula, char *archivo_hash_bin);


//Guardar y cargar el hash de los offsets en el archivo binario, para no perder la información del hash al cerrar el programa
void guardar_hash_en_binario( char *archivo_bin);
void cargar_hash_desde_binario( char *archivo_bin);


// Función para buscar una película por su título principal utilizando la tabla hash
struct Movie buscar_por_nombre(char *nombre , FILE *archivo_bin);
// Función para buscar una película por múltiples filtros, utilizando la tabla hash para optimizar la búsqueda
struct Movie buscar_por_filtros(struct Movie filtros, FILE *archivo_bin);

#endif 

