#include <stdio.h>
#include <string.h>
#include "Hash.h"



int main(int argc, char const *argv[])
{


    // inicializar_hash_table();
    // convertir_tsv_a_binario("title.basics.tsv", "peliculas.bin");
    // guardar_hash_en_binario("hash.bin");
    // printf("Hash guardado en hash.bin\n");


    cargar_hash_desde_binario("hash.bin");
    
    FILE *peliculas = fopen("peliculas.bin", "rb");
    
    struct Movie resultado = buscar_por_nombre("Spain", peliculas);
    
    if (resultado.primaryTitle[0] == '\0') {
        printf("No encontrado\n");
    } else {
        printf("Encontrado por nombre:\n");
        imprimir_pelicula(resultado);
    }
    
    


    struct Movie filtros;
    strcpy(filtros.titleType, "");// Ignorar este filtro
    strcpy(filtros.primaryTitle, "Spain");
    strcpy(filtros.originalTitle, ""); // Ignorar este filtro
    filtros.isAdult = -1; 
    filtros.startYear = 2006;
    filtros.runtimeMinutes = -1; // Ignorar este filtro
    strcpy(filtros.genres, ""); // Ignorar este filtro


    //buscar por filtros
    struct Movie resultado_filtros = buscar_por_filtros(filtros, peliculas);
    if (resultado_filtros.primaryTitle[0] == '\0') {
        printf("No encontrado\n");
    } else {
        //imprimir el resultado con filtros dados
        printf("Encontrado por filtros:\n");
        imprimir_pelicula(resultado_filtros);
    }


    return 0;


    
}


