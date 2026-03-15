#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include "imdb.h"

/* Colores ANSI para la terminal */
#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"

/* Inserta una pelicula nueva en el dataset via shm */
void agregarPelicula(Shared_Memory *shm) {
    Movie nueva;

    printf(YELLOW "Titulo principal: " RESET);
    fgets(nueva.primaryTitle, sizeof(nueva.primaryTitle), stdin);
    nueva.primaryTitle[strcspn(nueva.primaryTitle, "\n")] = '\0'; // strcspn encuentra el \n y lo reemplaza con \0

    printf(YELLOW "Titulo original: " RESET);
    fgets(nueva.originalTitle, sizeof(nueva.originalTitle), stdin);
    nueva.originalTitle[strcspn(nueva.originalTitle, "\n")] = '\0';

    printf(YELLOW "Tipo (movie/tvseries/short/...): " RESET);
    fgets(nueva.titleType, sizeof(nueva.titleType), stdin);
    nueva.titleType[strcspn(nueva.titleType, "\n")] = '\0';

    printf(YELLOW "Es adulto? (1/0): " RESET);
    char adulto[4];
    fgets(adulto, sizeof(adulto), stdin);
    nueva.isAdult = atoi(adulto);

    printf(YELLOW "Anio de inicio (N si no aplica): " RESET);
    char anio[8];
    fgets(anio, sizeof(anio), stdin);
    anio[strcspn(anio, "\n")] = '\0';
    if (strcmp(anio, "N") == 0) {
        nueva.startYear = -1;
    } else {
        nueva.startYear = atoi(anio);
    }

    printf(YELLOW "Duracion en minutos (N si no aplica): " RESET);
    char duracion[8];
    fgets(duracion, sizeof(duracion), stdin);
    duracion[strcspn(duracion, "\n")] = '\0';
    if (strcmp(duracion, "N") == 0) {
        nueva.runtimeMinutes = -1;
    } else {
        nueva.runtimeMinutes = atoi(duracion);
    }

    printf(YELLOW "Generos (Action,Drama,...): " RESET);
    fgets(nueva.genres, sizeof(nueva.genres), stdin);
    nueva.genres[strcspn(nueva.genres, "\n")] = '\0';

    nueva.next_offset = -1; // dataProgram lo actualiza al insertar

    /* Flujo: copiar pelicula a shm, despertar a dataProgram y esperar confirmacion */
    shm->movie = nueva;
    shm->query.searchCriteria = ADD_MOVIE;
    sem_post(&shm->sem_dp); // sem_post despierta a dataProgram
    sem_wait(&shm->sem_ui); // sem_wait duerme a ui hasta recibir confirmacion

    printf(GREEN "\nPelicula agregada exitosamente.\n" RESET);
}

/* Muestra el resultado de la busqueda — si es NA pregunta si agregar */
void mostrarResultado(Shared_Memory *shm) {
    if (shm->found == 0) {
        printf(RED "\nNA - Pelicula no encontrada.\n" RESET);

        printf(YELLOW "Desea agregar esta pelicula? (S/N): " RESET);
        char respuesta[4];
        fgets(respuesta, sizeof(respuesta), stdin);
        respuesta[strcspn(respuesta, "\n")] = '\0';

        if (strcmp(respuesta, "S") == 0) {
            agregarPelicula(shm);
        }
        return;
    }

    /* Mostrar campos del registro encontrado */
    printf(CYAN "\n╔══════════ Resultado ══════════╗\n" RESET);
    printf(YELLOW "  Tipo:      " RESET "%s\n", shm->movie.titleType);
    printf(YELLOW "  Titulo:    " RESET "%s\n", shm->movie.primaryTitle);
    printf(YELLOW "  Original:  " RESET "%s\n", shm->movie.originalTitle);
    printf(YELLOW "  Adultos:   " RESET "%s\n", shm->movie.isAdult ? "Si" : "No");
    printf(YELLOW "  Año:      " RESET "%d\n", shm->movie.startYear);
    printf(YELLOW "  Duracion:  " RESET "%d min\n", shm->movie.runtimeMinutes);
    printf(YELLOW "  Generos:   " RESET "%s\n", shm->movie.genres);
    printf(CYAN "╚═══════════════════════════════╝\n" RESET);
}

/* Busqueda simple por titulo — sin filtros */
void buscarPorTitulo(Shared_Memory *shm) {
    char titulo[SIZE_TITLES];

    printf(YELLOW "Ingrese el titulo que desea buscar: " RESET);
    fgets(titulo, sizeof(titulo), stdin);
    titulo[strcspn(titulo, "\n")] = '\0';

    if (strlen(titulo) == 0) {
        printf(RED "El titulo no puede estar vacio.\n" RESET);
        return;
    }

    /* Llenar query sin filtros — dataProgram usara buscar_por_nombre */
    shm->query.searchCriteria = SEARCH;
    strcpy(shm->query.primaryTitle, titulo);
    strcpy(shm->query.filterType,   "N");
    shm->query.filterYear = -1;
    strcpy(shm->query.filterGenre,  "N");

    /* Flujo: despertar a dataProgram y esperar resultado */
    sem_post(&shm->sem_dp);
    sem_wait(&shm->sem_ui);
    mostrarResultado(shm);
}

/* Busqueda por titulo con filtros opcionales de tipo, anio y genero */
void buscarConFiltros(Shared_Memory *shm) {
    char titulo[SIZE_TITLES];
    char tipo[SIZE_TYPE];
    char anio[8];
    char genero[SIZE_GENRES];

    printf(YELLOW "Ingrese el titulo: " RESET);
    fgets(titulo, sizeof(titulo), stdin);
    titulo[strcspn(titulo, "\n")] = '\0';

    if (strlen(titulo) == 0) {
        printf(RED "El titulo no puede estar vacio.\n" RESET);
        return;
    }

    /* Pedir filtros opcionales — N para omitir */
    printf(YELLOW "Tipo (movie/tvseries/short — N para omitir): " RESET);
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = '\0';

    printf(YELLOW "Anio de inicio (N para omitir): " RESET);
    fgets(anio, sizeof(anio), stdin);
    anio[strcspn(anio, "\n")] = '\0';

    printf(YELLOW "Genero (Action/Drama/... — N para omitir): " RESET);
    fgets(genero, sizeof(genero), stdin);
    genero[strcspn(genero, "\n")] = '\0';

    /* Llenar query — dataProgram usara buscar_por_filtros */
    shm->query.searchCriteria = SEARCH;
    strcpy(shm->query.primaryTitle, titulo);

    if (strcmp(tipo, "N") == 0) {
        strcpy(shm->query.filterType, "N");
    } else {
        strcpy(shm->query.filterType, tipo);
    }

    if (strcmp(anio, "N") == 0) {
        shm->query.filterYear = -1;
    } else {
        shm->query.filterYear = atoi(anio);
    }

    if (strcmp(genero, "N") == 0) {
        strcpy(shm->query.filterGenre, "N");
    } else {
        strcpy(shm->query.filterGenre, genero);
    }

    /* Flujo: despertar a dataProgram y esperar resultado */
    sem_post(&shm->sem_dp);
    sem_wait(&shm->sem_ui);
    mostrarResultado(shm);
}

/* Avisa a dataProgram que debe terminar */
void salir(Shared_Memory *shm) {
    shm->query.searchCriteria = EXIT;
    sem_post(&shm->sem_dp);
    printf(CYAN "\nHasta luego!\n" RESET);
}

/* Menu de opciones */
void mostrarMenu() {
    printf(CYAN "\n╔══════════ Menu ══════════╗\n" RESET);
    printf(BLUE "  1. Buscar por titulo\n" RESET);
    printf(BLUE "  2. Buscar por titulo + filtros\n" RESET);
    printf(RED  "  3. Salir\n" RESET);
    printf(CYAN "╚══════════════════════════╝\n" RESET);
    printf(YELLOW "Opcion: " RESET);
}

/* Mensaje de bienvenida e instrucciones */
void mostrarIntro() {
    printf(CYAN);
    printf("╔════════════════════════════════════╗\n");
    printf("║         IMDB Movie Search          ║\n");
    printf("╚════════════════════════════════════╝\n");
    printf(RESET);

    printf(BLUE);
    printf("\nBienvenido! Este programa te permitira buscar\n");
    printf("peliculas y series en la base de datos IMDB.\n");
    printf(RESET);

    printf(YELLOW "\nInstrucciones:\n" RESET);
    printf("  - Opcion 1: busqueda solo por titulo\n");
    printf("  - Opcion 2: busqueda por titulo + filtros opcionales\n");
    printf("  - En filtros, escribe N para omitir\n");
    printf("  - Si no se encuentra, puede agregar la pelicula/serie al dataset\n");
}

int main() {
    /* shmget busca el segmento creado por dataProgram usando la misma clave SHM_KEY */
    int shm_id = shmget(SHM_KEY, sizeof(Shared_Memory), 0666);
    if (shm_id == -1) {
        perror("Error al conectarse a memoria compartida");
        exit(-1);
    }

    /* shmat mapea el segmento al espacio de direcciones de este proceso */
    Shared_Memory *shm = (Shared_Memory *) shmat(shm_id, 0, 0);
    if (shm == (void *)(-1)) {
        perror("Error en shmat");
        exit(-1);
    }

    mostrarIntro();

    char input[4];
    int  opcion = 0;

    do {
        mostrarMenu();
        fgets(input, sizeof(input), stdin);
        opcion = atoi(input);

        switch (opcion) {
            case 1: buscarPorTitulo(shm);  break;
            case 2: buscarConFiltros(shm); break;
            case 3: salir(shm);            break;
            default:
                printf(RED "Opcion invalida.\n" RESET);
        }
    } while (opcion != 3);

    /* shmdt desconecta el puntero — dataProgram elimina el segmento con shmctl */
    shmdt(shm);
    return 0;
}



