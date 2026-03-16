#ifndef IMDB_H
#define IMDB_H

#include <semaphore.h>
#include <stdio.h>

/* Tamaños de los campos */
#define SIZE_TITLES  128
#define SIZE_TYPE    32
#define SIZE_GENRES  64
#define HASH_SIZE 500009  //Cantidad de espacio que tendra el array del hash

/* Clave de memoria compartida */
#define SHM_KEY 0x494D4242

/* Acciones que ui le pide a dataProgram */
typedef enum {
    SEARCH,
    ADD_MOVIE,
    EXIT
} Criteria;

/* Query que ui escribe en la shm */
typedef struct {
    Criteria searchCriteria;
    char primaryTitle[SIZE_TITLES]; // Obligatorio
    char filterType[SIZE_TYPE];     // "" = sin filtro
    int  filterYear;                // -1 = sin filtro
    char filterGenre[SIZE_GENRES];  // "" = sin filtro
} Query;

/* Registro de una película del dataset */
typedef struct {
    char titleType[SIZE_TYPE];
    char primaryTitle[SIZE_TITLES];
    char originalTitle[SIZE_TITLES];
    int  isAdult;
    int  startYear;
    int  runtimeMinutes;
    char genres[SIZE_GENRES];
    long next_offset;               // -1 = fin de lista enlazada
} Movie;

/* Memoria compartida entre ui y dataProgram */
typedef struct {
    sem_t sem_dp;                   // señal para dataProgram
    sem_t sem_ui;                   // señal para ui
    Query query;
    Movie movie;
    int   found;                    // 1 = encontrado, 0 = NA
    double search_time_ms;          // tiempo de busqueda en ms
} Shared_Memory;

/* Prototipos de funciones */
// Declaración de la tabla hash como un array global de long, donde cada posición almacena el offset del primer elemento en la cadena de colisiones
extern long hash_table[HASH_SIZE];

// Función para imprimir los detalles de una película
void imprimir_pelicula( Movie pelicula);

// Función para calcular el hash de una cadena de caracteres 
unsigned int calcular_hash( char *str);
//Inicializar la tabla hash con -1 para indicar que cada posición esta vacia
void inicializar_hash_table();

int leer_linea_tsv(FILE *archivo,  Movie *pelicula);
// retorna 1 si leyó bien, 0 si línea malformada o fin de archivo

void escribir_pelicula_bin( Movie *pelicula, FILE *archivo_bin);
// escribe la struct y actualiza el hash

void convertir_tsv_a_binario(char *archivo_tsv, char *archivo_bin);
// el loop con malloc/free

void insertar_pelicula_en_binario( Movie pelicula, char *archivo_hash_bin);


//Guardar y cargar el hash de los offsets en el archivo binario, para no perder la información del hash al cerrar el programa
void guardar_hash_en_binario( char *archivo_bin);
void cargar_hash_desde_binario( char *archivo_bin);


// Función para buscar una película por su título principal utilizando la tabla hash
Movie buscar_por_nombre(char *nombre , FILE *archivo_bin);
// Función para buscar una película por múltiples filtros, utilizando la tabla hash para optimizar la búsqueda
Movie buscar_por_filtros(Movie filtros, FILE *archivo_bin);


#endif

