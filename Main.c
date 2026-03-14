#include <stdio.h>
#include <string.h>
#include "Hash.h"

char linea [1024];

int main(int argc, char const *argv[])
{
    printf("Hello, World!\n");
    FILE *file = fopen("title.basics.tsv", "r");
    if (file == NULL) {
        perror("Error oabriendo el archivo");
        return 1;
    }

    fgets(linea, sizeof(linea), file);
    fgets(linea, sizeof(linea), file);
    char *pruebitastrtok = strtok(linea, "\t");
    printf("%s", pruebitastrtok);
    printf("\n");
    //printf("Acabe\n");

    //intertar 30 nombres de prueba
    insertar_ordenado(1, "Juan");
    insertar_ordenado(2, "Pedro");
    insertar_ordenado(3, "Maria");
    insertar_ordenado(4, "Ana");
    insertar_ordenado(5, "Luis");
    insertar_ordenado(6, "Carlos");
    insertar_ordenado(7, "Sofia");
    insertar_ordenado(8, "Miguel");
    insertar_ordenado(9, "Lucia");
    insertar_ordenado(10, "Diego");
    insertar_ordenado(11, "Elena");
    insertar_ordenado(12, "Jorge");
    insertar_ordenado(13, "Laura");
    insertar_ordenado(14, "Andres");
    insertar_ordenado(15, "Carmen");
    insertar_ordenado(16, "Roberto");
    insertar_ordenado(17, "Isabel");
    insertar_ordenado(18, "Fernando");
    insertar_ordenado(19, "Patricia");
    insertar_ordenado(20, "Diego");
    insertar_ordenado(21, "Silvia");
    insertar_ordenado(22, "Ricardo");
    insertar_ordenado(23, "Martha");
    insertar_ordenado(24, "Alejandro");
    insertar_ordenado(25, "Diana");
    insertar_ordenado(26, "Santiago");
    insertar_ordenado(27, "Claudia");
    insertar_ordenado(28, "Mauricio");
    insertar_ordenado(29, "Beatriz");
    insertar_ordenado(30, "Guillermo");

    //imprimir las listas enlazadas de la tabla hash
        for (int i = 0; i < HASH_SIZE; i++) {
            struct Nodo *actual = hash_table[i];
            if (actual != NULL) {
                printf("Índice %d:\n", i);
                while (actual != NULL) {
                    printf("  Offset: %ld, Nombre: %s\n", actual->offset, actual->nombre);
                    actual = actual->siguiente;
                }
            }
        }
    return 0;

}

