#include <stdio.h>
#include "imdb.h"

int main() {
    inicializar_hash_table();
    convertir_tsv_a_binario("title.basics.tsv", "peliculas.bin");
    guardar_hash_en_binario("hash.bin");
    printf("Conversion terminada!\n");
    return 0;
}