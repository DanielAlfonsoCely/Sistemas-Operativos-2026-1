#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <libgen.h>

#define RESET     "\033[0m"
#define AZUL      "\033[1;34m"
#define VERDE     "\033[1;32m"
#define MAGENTA   "\033[1;35m"
#define AMARILLO  "\033[1;33m"
#define ROJO      "\033[1;31m"

void crear_carpeta() {
    system("mkdir -p /home/data/backup/");
    system("chmod 777 /home/data/backup/");

    struct stat info;
    if (stat("/home/data/backup/", &info) == -1) {
        fprintf(stderr, ROJO);
        perror("Error: no se pudo crear la carpeta de backup");
        fprintf(stderr, RESET);
        printf(ROJO "Intenta ejecutar: sudo mkdir -p /home/data/backup/\n" RESET);
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    crear_carpeta();

    struct stat file_stat;
    char comando[500];
    char fecha[100];

    if (argc != 2) {
        printf(ROJO "Uso: %s <archivo_a_monitorizar>\n" RESET, argv[0]);
        return 1;
    }

    if (stat(argv[1], &file_stat) == -1) {
        fprintf(stderr, ROJO);
        perror("Error: no se puede acceder al archivo");
        fprintf(stderr, RESET);
        return 1;
    }

    time_t initial_mtime = file_stat.st_mtime;

    printf(AZUL "Monitoreando el archivo: " MAGENTA "%s\n" RESET, argv[1]);
    printf(AZUL "Presiona Ctrl+C para detener.\n" RESET);

    while (1) {
        sleep(1);

        if (stat(argv[1], &file_stat) == -1) {
            fprintf(stderr, ROJO);
            perror("Error al leer el archivo");
            fprintf(stderr, RESET);
            return 1;
        }   

        if (file_stat.st_mtime != initial_mtime) {
            printf(AMARILLO "Cambio detectado. Creando backup...\n" RESET);

            time_t ahora = time(NULL);
            struct tm *t = localtime(&ahora);
            strftime(fecha, sizeof(fecha), "%Y%m%d_%H%M%S", t);

            snprintf(comando, sizeof(comando), "cp %s /home/data/backup/%s_%s", argv[1], fecha, basename(argv[1]));
            
            if (system(comando) == 0) {
                printf(VERDE "Backup creado: %s_%s\n" RESET, fecha, basename(argv[1]));
            } else {
                fprintf(stderr, ROJO);
                perror("Error al crear el backup");
                fprintf(stderr, RESET);
            }

            initial_mtime = file_stat.st_mtime;
        }
    }

    return 0;
}
