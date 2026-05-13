#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imdb.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/* Colores ANSI para la terminal */
#define RESET   "\033[0m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"

#define PORT    3535

/* Inserta una pelicula nueva en el dataset via shm */
void agregarPelicula(int fd , Response *response) {
    Movie nueva;

    /* Campos obligatorios — loop hasta que el usuario ingrese algo */
    do {
        printf(YELLOW "Titulo principal: " RESET);
        fgets(nueva.primaryTitle, sizeof(nueva.primaryTitle), stdin);
        nueva.primaryTitle[strcspn(nueva.primaryTitle, "\n")] = '\0';
        if (strlen(nueva.primaryTitle) == 0)
            printf(RED "Este campo no puede estar vacio.\n" RESET);
    } while (strlen(nueva.primaryTitle) == 0);

    do {
        printf(YELLOW "Titulo original: " RESET);
        fgets(nueva.originalTitle, sizeof(nueva.originalTitle), stdin);
        nueva.originalTitle[strcspn(nueva.originalTitle, "\n")] = '\0';
        if (strlen(nueva.originalTitle) == 0)
            printf(RED "Este campo no puede estar vacio.\n" RESET);
    } while (strlen(nueva.originalTitle) == 0);

    do {
        printf(YELLOW "Tipo (movie/short/tvSeries/tvEpisode/tvMovie/tvMiniSeries/tvPilot/tvShort/tvSpecial/video/videoGame): " RESET);
        fgets(nueva.titleType, sizeof(nueva.titleType), stdin);
        nueva.titleType[strcspn(nueva.titleType, "\n")] = '\0';
        if (strlen(nueva.titleType) == 0)
            printf(RED "Este campo no puede estar vacio.\n" RESET);
    } while (strlen(nueva.titleType) == 0);

    do {
        printf(YELLOW "Es adulto? (1/0): " RESET);
        char adulto[4];
        fgets(adulto, sizeof(adulto), stdin);
        adulto[strcspn(adulto, "\n")] = '\0';
        if (strlen(adulto) == 0) {
            printf(RED "Este campo no puede estar vacio.\n" RESET);
        } else {
            nueva.isAdult = atoi(adulto);
            break;
        }
    } while (1);

    do {
        printf(YELLOW "Anio de inicio: " RESET);
        char anio[8];
        fgets(anio, sizeof(anio), stdin);
        anio[strcspn(anio, "\n")] = '\0';
        if (strlen(anio) == 0) {
            printf(RED "Este campo no puede estar vacio.\n" RESET);
        } else {
            nueva.startYear = atoi(anio);
            if (nueva.startYear <= 0) {
                printf(RED "El anio debe ser un entero positivo.\n" RESET);
            } else {
                break;
            }
        }
    } while (1);

    /* Duracion — opcional, Enter = N/A */
    printf(YELLOW "Duracion en minutos (Enter si no aplica): " RESET);
    char duracion[8];
    fgets(duracion, sizeof(duracion), stdin);
    duracion[strcspn(duracion, "\n")] = '\0';
    if (strlen(duracion) == 0) {
        nueva.runtimeMinutes = -1;
    } else {
        nueva.runtimeMinutes = atoi(duracion);
        if (nueva.runtimeMinutes <= 0) {
            printf(RED "Duracion invalida, se guardara como N/A.\n" RESET);
            nueva.runtimeMinutes = -1;
        }
    }

    do {
        printf(YELLOW "Generos (Action,Drama,...): " RESET);
        fgets(nueva.genres, sizeof(nueva.genres), stdin);
        nueva.genres[strcspn(nueva.genres, "\n")] = '\0';
        if (strlen(nueva.genres) == 0)
            printf(RED "Este campo no puede estar vacio.\n" RESET);
    } while (strlen(nueva.genres) == 0);

    nueva.next_offset = -1; // dataProgram lo actualiza al insertar

    response->movie = nueva;
    response->query.searchCriteria = ADD_MOVIE;


    send(fd, (void *)response, sizeof(Response), 0);

    recv(fd, (void *)response, sizeof(Response), 0);

    if (response->found == 1) {
        printf(GREEN "\nPelicula agregada exitosamente.\n" RESET);
    } else {
        printf(RED "\nHubo un error al agregar la pelicula.\n" RESET);
    }
}

/* Muestra el resultado de la busqueda — si es NA pregunta si agregar */
void mostrarResultado(int fd, Response *response) {
    if (response->found == 0) {
        printf(RED "\nNA - Pelicula no encontrada.\n" RESET);

        printf(YELLOW "Desea agregar esta pelicula? (S/N): " RESET);
        char respuesta[4];
        fgets(respuesta, sizeof(respuesta), stdin);
        respuesta[strcspn(respuesta, "\n")] = '\0';

        if (strcmp(respuesta, "S") == 0) {
            agregarPelicula(fd, response);
        }
        return;
    }

    /* Mostrar campos del registro encontrado */
    printf(CYAN "\n╔══════════ Resultado ══════════╗\n" RESET);
    printf(YELLOW "  Tipo:      " RESET "%s\n", response->movie.titleType);
    printf(YELLOW "  Titulo:    " RESET "%s\n", response->movie.primaryTitle);
    printf(YELLOW "  Original:  " RESET "%s\n", response->movie.originalTitle);
    printf(YELLOW "  Adultos:   " RESET "%s\n", response->movie.isAdult ? "Si" : "No");
    printf(YELLOW "  Anio:      " RESET "%d\n", response->movie.startYear);
    if (response->movie.runtimeMinutes == -1) {
        printf(YELLOW "  Duracion:  " RESET "N/A\n");
    } else {
        printf(YELLOW "  Duracion:  " RESET "%d min\n", response->movie.runtimeMinutes);
    }
    printf(YELLOW "  Generos:   " RESET "%s\n", response->movie.genres);
    printf(CYAN "╚═══════════════════════════════╝\n" RESET);
    printf(YELLOW "  Tiempo de busqueda: " RESET "%.2f ms\n", response->search_time_ms);
}

/* Busqueda simple por titulo — sin filtros */
void buscarPorTitulo(int fd , Response *response) {
    char titulo[SIZE_TITLES];

    printf(YELLOW "Ingrese el titulo que desea buscar: " RESET);
    fgets(titulo, sizeof(titulo), stdin);
    titulo[strcspn(titulo, "\n")] = '\0';

    if (strlen(titulo) == 0) {
        printf(RED "El titulo no puede estar vacio.\n" RESET);
        return;
    }

    /* Llenar query sin filtros — dataProgram usara buscar_por_nombre */
    response->query.searchCriteria = SEARCH;
    strcpy(response->query.primaryTitle, titulo);
    strcpy(response->query.filterType,  "");
    response->query.filterYear = -1;
    strcpy(response->query.filterGenre, "");


    send(fd, (void *)response, sizeof(Response), 0);

    recv(fd, (void *)response, sizeof(Response), 0);

    mostrarResultado(fd, response);
}

/* Busqueda por titulo con filtros opcionales de tipo, anio y genero */
void buscarConFiltros(int fd, Response *response) {
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

    /* Pedir filtros opcionales — Enter para omitir */
    printf(YELLOW "Tipo (movie/short/tvSeries/tvEpisode/tvMovie/tvMiniSeries/tvPilot/tvShort/tvSpecial/video/videoGame — Enter para omitir): " RESET);
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = '\0';

    printf(YELLOW "Anio de inicio (Enter para omitir): " RESET);
    fgets(anio, sizeof(anio), stdin);
    anio[strcspn(anio, "\n")] = '\0';

    printf(YELLOW "Genero (Action/Drama/... — Enter para omitir): " RESET);
    fgets(genero, sizeof(genero), stdin);
    genero[strcspn(genero, "\n")] = '\0';

    /* Llenar query — dataProgram usara buscar_por_filtros */
    response->query.searchCriteria = SEARCH;
    strcpy(response->query.primaryTitle, titulo);

    if (strlen(tipo) == 0) {
        strcpy(response->query.filterType, "");
    } else {
        strcpy(response->query.filterType, tipo);
    }

    if (strlen(anio) == 0) {
        response->query.filterYear = -1;
    } else {
        response->query.filterYear = atoi(anio);
    }

    if (strlen(genero) == 0) {
        strcpy(response->query.filterGenre, "");
    } else {
        strcpy(response->query.filterGenre, genero);
    }

    send(fd, (void *)response, sizeof(Response), 0);

    recv(fd, (void *)response, sizeof(Response), 0);

    mostrarResultado(fd, response);
}

/* Avisa a dataProgram que debe terminar */
void salir(int fd, Response *response) {
    response->query.searchCriteria = EXIT;
    send(fd, (void *)response, sizeof(Response), 0);
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
    printf("  - En filtros, presiona Enter para omitir\n");
    printf("  - Si no se encuentra, puede agregar la pelicula/serie al dataset\n");
}

int main() {
    
    int fd;
    struct sockaddr_in server;
    socklen_t addrlen;
    int r;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Error en bind");
        exit(-1);
    }
    
    server.sin_family = AF_INET;    //ipv4
    server.sin_port = htons(PORT);  //endianismo
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); //cadena de dir ip a ip enteros
    bzero(&(server.sin_zero), 8);

    addrlen = sizeof(struct sockaddr);

    r = connect(fd, (struct sockaddr *)&server, addrlen);

    if(r == -1){
        perror("Error en connect");
        exit(-1);
    }


    Response response;

    mostrarIntro();

    char input[4];
    int  opcion = 0;

    do {
        mostrarMenu();
        fgets(input, sizeof(input), stdin);
        opcion = atoi(input);

        switch (opcion) {
            case 1: buscarPorTitulo(fd ,&response);  break;
            case 2: buscarConFiltros(fd, &response); break;
            case 3: salir(fd, &response);            break;
            default:
                printf(RED "Opcion invalida.\n" RESET);
        }
    } while (opcion != 3);


    close(fd);
    return 0;
}



