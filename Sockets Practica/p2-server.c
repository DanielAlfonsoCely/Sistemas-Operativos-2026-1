#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "imdb.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/time.h>

 
#define PORT     3535
#define BACKLOG  8
#define MAX_CLIENTES 32

FILE *peliculas;  // global, accesible por todos los hilos
/* Proceso principal de busqueda — crea la shm, espera queries de ui y responde */

/* mutexes y contador de clientes */
pthread_mutex_t mutex_archivo;   // protege peliculas.bin al buscar/insertar
pthread_mutex_t mutex_log;       // protege server.log
pthread_mutex_t mutex_clientes;  // protege num_clientes
int num_clientes = 0;

/* Prototipo necesario porque handle_client se define después de main */
void *handle_client(void *arg);



/* ── escribir_log ─────────────────────────────────────────────────────────
   Formato:
   [20260509T143022] Cliente [IP] [SEARCH - titulo]
   [20260509T143022] Cliente [IP] [SEARCH_FILTER - titulo - tipo/anio]
   [20260509T143022] Cliente [IP] [ADD_MOVIE - titulo]
   ───────────────────────────────────────────────────────────────────────── */
void escribir_log(Query q, const char *ip_cliente) {
    pthread_mutex_lock(&mutex_log);
 
    FILE *log = fopen("server.log", "a");
    if (log == NULL) {
        perror("Error al abrir server.log");
        pthread_mutex_unlock(&mutex_log);
        return;
    }
 
    /* Timestamp YYYYMMDDTHHmmss */
    time_t ahora = time(NULL);
    struct tm *t = localtime(&ahora);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%dT%H%M%S", t);
 
    if (q.searchCriteria == SEARCH &&
        q.filterYear == -1 &&
        strlen(q.filterType)  == 0 &&
        strlen(q.filterGenre) == 0) {
 
        fprintf(log, "[%s] Cliente [%s] [SEARCH - %s]\n",
                timestamp, ip_cliente, q.primaryTitle);
 
    } else if (q.searchCriteria == SEARCH) {
 
        char anio_str[8];
        if (q.filterYear == -1)
            strcpy(anio_str, "N/A");
        else
            snprintf(anio_str, sizeof(anio_str), "%d", q.filterYear);
 
        fprintf(log, "[%s] Cliente [%s] [SEARCH_FILTER - %s - %s/%s]\n",
                timestamp, ip_cliente,
                q.primaryTitle,
                strlen(q.filterType) > 0 ? q.filterType : "N/A",
                anio_str);
 
    } else if (q.searchCriteria == ADD_MOVIE) {
 
        fprintf(log, "[%s] Cliente [%s] [ADD_MOVIE - %s]\n",
                timestamp, ip_cliente, q.primaryTitle);
    }
 
    fclose(log);
    pthread_mutex_unlock(&mutex_log);
}

int main() {

    int sockfd, r;
    int sockfdc;
    struct sockaddr_in server, cliente;
    socklen_t addrlen, addrlen_c;

     /* inicializar los 3 mutexes antes de aceptar clientes */
    pthread_mutex_init(&mutex_archivo,  NULL);
    pthread_mutex_init(&mutex_log,      NULL);
    pthread_mutex_init(&mutex_clientes, NULL);

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

        addrlen_c = sizeof(struct sockaddr_in);
        sockfdc = accept(sockfd, (struct sockaddr *)&cliente, &addrlen_c);
        if(sockfdc == -1){
            perror("Error en accept");
            exit(-1);
        }

        /* Verificar límite de 32 clientes */
        pthread_mutex_lock(&mutex_clientes);
        if (num_clientes >= MAX_CLIENTES) {
            pthread_mutex_unlock(&mutex_clientes);
            printf("Limite de clientes alcanzado, conexion rechazada.\n");
            close(sockfdc);
            continue;
        }
        num_clientes++;
        pthread_mutex_unlock(&mutex_clientes);

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

    pthread_mutex_destroy(&mutex_archivo);
    pthread_mutex_destroy(&mutex_log);
    pthread_mutex_destroy(&mutex_clientes);

    return 0;
}


void *handle_client(void *arg) {
    int sockfdc = *(int *)arg;
    free(arg);  // liberar memoria temporal
    // Aquí se manejará la comunicación con el cliente conectado a sockfdc
    // Se pueden recibir queries, procesarlas y enviar respuestas
    // Este código se ejecutará en un proceso o hilo separado para cada cliente

    /* Obtener IP del cliente para el log */
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    char ip_cliente[INET_ADDRSTRLEN];
    getpeername(sockfdc, (struct sockaddr *)&addr, &len);
    inet_ntop(AF_INET, &addr.sin_addr, ip_cliente, INET_ADDRSTRLEN);

    int corriendo = 1;
    //dentro del switch se debe hacer el respectivo send al cliente con la respuesta
    while(corriendo) {
        // Response = Aca se debe hacer el rcv del cliente y se guarda en variable de tipo Response
        /* Recv — recibir la estructura Response completa del cliente */
        Response response;
        if (recv(sockfdc, (void *)&response, sizeof(Response), MSG_WAITALL) <= 0) {
            perror("Error al recibir Response");
            pthread_mutex_lock(&mutex_clientes);
            num_clientes--;
            pthread_mutex_unlock(&mutex_clientes);
            close(sockfdc);
            return NULL;
        }
    

        switch (response.query.searchCriteria) {

            case SEARCH: {

                struct timespec t_start, t_end;
                
                clock_gettime(CLOCK_MONOTONIC, &t_start);

                /* Mutex para proteger peliculas.bin */
                pthread_mutex_lock(&mutex_archivo);

                /* Sin filtros: usar buscar_por_nombre
                Con filtros: armar struct Movie y usar buscar_por_filtros */
                if (response.query.filterYear == -1 &&
                    strlen(response.query.filterType)  == 0 &&
                    strlen(response.query.filterGenre) == 0) {

                    response.movie = buscar_por_nombre(response.query.primaryTitle, peliculas);

                } else {

                    Movie filtros;
                    memset(&filtros, 0, sizeof(Movie));
                    strcpy(filtros.primaryTitle,  response.query.primaryTitle);
                    strcpy(filtros.titleType,     response.query.filterType);
                    filtros.startYear           = response.query.filterYear;
                    strcpy(filtros.genres,        response.query.filterGenre);
                    // No se filtra por estos campos
                    filtros.isAdult             = -1;
                    filtros.runtimeMinutes      = -1;
                    strcpy(filtros.originalTitle, "");

                    response.movie = buscar_por_filtros(filtros, peliculas);
                }

                pthread_mutex_unlock(&mutex_archivo);

                clock_gettime(CLOCK_MONOTONIC, &t_end);
                response.search_time_ms = (t_end.tv_sec - t_start.tv_sec) * 1000.0 + (t_end.tv_nsec - t_start.tv_nsec) / 1e6;
                    
                /* Indicar a ui si se encontro o no */
                response.found = (response.movie.primaryTitle[0] != '\0') ? 1 : 0;

                /* Send — devolver Response con resultado al cliente */
                send(sockfdc, (void *)&response, sizeof(Response), 0);
    
                escribir_log(response.query, ip_cliente);
                break;
                
            }

            case ADD_MOVIE: {

                /* Mutex para proteger peliculas.bin al insertar */
                pthread_mutex_lock(&mutex_archivo);
                insertar_pelicula_en_binario(response.movie, "peliculas.bin");
                pthread_mutex_unlock(&mutex_archivo);
    
                /* Send — notificar al cliente que se agregó */
                response.found = 1;
                send(sockfdc, (void *)&response, sizeof(Response), 0);
    
                escribir_log(response.query, ip_cliente);
                break;
            }

            case EXIT:{
                /* Decrementar contador al terminar */
                pthread_mutex_lock(&mutex_clientes);
                num_clientes--;
                pthread_mutex_unlock(&mutex_clientes);
        
                close(sockfdc);
                corriendo = 0;
                break;
                    
            }
        }
    }
    

    

    return NULL;
    
}
