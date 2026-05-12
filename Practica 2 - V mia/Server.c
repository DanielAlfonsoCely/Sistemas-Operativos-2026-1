#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <linux/time.h>
#include "imdb.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT    3535
#define BACKLOG 8
FILE *peliculas;  // global, accesible por todos los hilos
/* Proceso principal de busqueda — crea la shm, espera queries de ui y responde */
int main() {

    int sockfd, r;
    int sockfdc;
    struct sockaddr_in server, cliente;
    socklen_t addrlen, addrlen_c;

    // crear socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("Error en socket");
        exit(-1);
    }

    server.sin_family = AF_INET;    //ipv4
    server.sin_port = htons(PORT);  //endianismo
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server.sin_zero), 8);
    addrlen = sizeof(struct sockaddr);
    //nombrar socket
    r = bind(sockfd, (struct sockaddr *)&server, addrlen);
    if(r == -1){
        perror("Error en bind");
        exit(-1);
    }

    r = listen(sockfd, BACKLOG);
    if(r == -1){
        perror("Error en listen");
        exit(-1);
    }

    
    
    // WARNING: send envía bytes, el programador debe verificar que se hayan enviado todos los bytes!!!



    /* Cargar indice hash desde archivo — los datos quedan en disco */
    cargar_hash_desde_binario("hash.bin");

    peliculas = fopen("peliculas.bin", "rb+");
    if (peliculas == NULL) {
        perror("Error al abrir peliculas.bin");
        exit(-1);
    }

    

    while (1) {

        sockfdc = accept(sockfd, (struct sockaddr *)&cliente, &addrlen_c);
        if(sockfdc == -1){
            perror("Error en accept");
            exit(-1);
        }

        int *sockfdc_ptr = malloc(sizeof(int));
        *sockfdc_ptr = sockfdc;

        

    //Manejar max 32 clientes


    pthread_t tid;  // variable temporal, se reusa cada vez
    pthread_create(&tid, NULL, handle_client, (void *)sockfdc_ptr  );
    pthread_detach(tid);  // el hilo se limpia solo al terminar

    }

    fclose(peliculas);
    close(sockfdc);
    close(sockfd);

    return 0;
}


void *handle_client(void *arg) {
    int sockfdc = *(int *)arg;
    free(arg);  // liberar memoria temporal
    // Aquí se manejará la comunicación con el cliente conectado a sockfdc
    // Se pueden recibir queries, procesarlas y enviar respuestas
    // Este código se ejecutará en un proceso o hilo separado para cada cliente


    response = // Aca se debe hacer el rcv del cliente y se guarda en variable de tipo Response

    //dentro del switch se debe hacer el respectivo send al cliente con la respuesta
    
    switch (response->query.searchCriteria) {

            case SEARCH: {

                struct timespec t_start, t_end;
                clock_gettime(CLOCK_MONOTONIC, &t_start);

                /* Sin filtros: usar buscar_por_nombre
                   Con filtros: armar struct Movie y usar buscar_por_filtros */
                if (response->query.filterYear == -1 &&
                    strlen(response->query.filterType)  == 0 &&
                    strlen(response->query.filterGenre) == 0) {

                    response->movie = buscar_por_nombre(response->query.primaryTitle, peliculas);

                } else {

                    Movie filtros;
                    strcpy(filtros.primaryTitle,  response->query.primaryTitle);
                    strcpy(filtros.titleType,     response->query.filterType);
                    filtros.startYear           = response->query.filterYear;
                    strcpy(filtros.genres,        response->query.filterGenre);
                    // No se filtra por estos campos
                    filtros.isAdult             = -1;
                    filtros.runtimeMinutes      = -1;
                    strcpy(filtros.originalTitle, "");

                    response->movie = buscar_por_filtros(filtros, peliculas);
                }

                clock_gettime(CLOCK_MONOTONIC, &t_end);
                response->search_time_ms = (t_end.tv_sec - t_start.tv_sec) * 1000.0 + (t_end.tv_nsec - t_start.tv_nsec) / 1e6;
                
                /* Indicar a ui si se encontro o no */
                response->found = (response->movie.primaryTitle[0] != '\0') ? 1 : 0;

                //sem_post(&shm->sem_ui); // despertar a ui con el resultado
                
            }

            case ADD_MOVIE: {
                /* ui ya lleno shm->movie — insertar en .bin y actualizar hash */
                insertar_pelicula_en_binario(response->movie, "peliculas.bin");
                response->found = 1;
                sem_post(&shm->sem_ui);
                
            }

            case EXIT:{
                corriendo = 0;
                
        }
    }

    return NULL;
    
    
    
}
