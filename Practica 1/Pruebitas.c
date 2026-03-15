#include <stdio.h>
#include <string.h>
#include "imdb.h"



int main(int argc, char const *argv[])
{
    // inicializar_hash_table(); // Inicializar la tabla hash antes de usarla
    
    // convertir_tsv_a_binario("title.basics.tsv", "peliculas.bin"); 
    
    // guardar_hash_en_binario("hash.bin");



    cargar_hash_desde_binario("hash.bin");
    
    FILE *peliculas = fopen("peliculas.bin", "rb");
    
    Movie resultado = buscar_por_nombre("Spain", peliculas);
    
    if (resultado.primaryTitle[0] == '\0') {
        printf("No encontrado\n");
    } else {
        printf("Encontrado por nombre:\n");
        imprimir_pelicula(resultado);
    }
    
    


    Movie filtros;
    strcpy(filtros.titleType, "N");// Ignorar este filtro
    strcpy(filtros.primaryTitle, "Spain");
    strcpy(filtros.originalTitle, "N"); // Ignorar este filtro
    filtros.isAdult = -1; 
    filtros.startYear = 2006;
    filtros.runtimeMinutes = -1; // Ignorar este filtro
    strcpy(filtros.genres, "N"); // Ignorar este filtro


    //buscar por filtros
    Movie resultado_filtros = buscar_por_filtros(filtros, peliculas);
    if (resultado_filtros.primaryTitle[0] == '\0') {
        printf("No encontrado\n");
    } else {
        //imprimir el resultado con filtros dados
        printf("Encontrado por filtros:\n");
        imprimir_pelicula(resultado_filtros);
    }

    //Crear una película de prueba para insertarla en el archivo binario y luego buscarla por nombre para verificar que se insertó correctamente
    Movie prueba;
    strcpy(prueba.primaryTitle, "Mi Pelicula");
    strcpy(prueba.originalTitle, "Mi Pelicula Original");
    prueba.isAdult = 0;
    prueba.startYear = 2020;
    prueba.runtimeMinutes = 120;
    strcpy(prueba.genres, "Drama");

    insertar_pelicula_en_binario(prueba, "peliculas.bin");

    Movie resultado2 = buscar_por_nombre("nose19fshdfskjadfhds", peliculas);

    if (resultado2.primaryTitle[0] == '\0') {
        printf("No encontrado\n");
    } else {
        imprimir_pelicula(resultado2);
    }
    
    fclose(peliculas);

    return 0;


    
}


