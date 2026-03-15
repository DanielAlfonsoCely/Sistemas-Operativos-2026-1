#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include "imdb.h"

/* Proceso principal de busqueda — crea la shm, espera queries de ui y responde */
int main() {

    /* shmget crea el segmento con IPC_CREAT — ui solo se conecta */
    int shm_id = shmget(SHM_KEY, sizeof(Shared_Memory), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Error al crear memoria compartida");
        exit(-1);
    }

    /* shmat mapea el segmento al espacio de direcciones de este proceso */
    Shared_Memory *shm = (Shared_Memory *) shmat(shm_id, 0, 0);
    if (shm == (void *)(-1)) {
        perror("Error en shmat");
        exit(-1);
    }

    /* Inicializar semaforos dentro de la shm */
    sem_init(&shm->sem_dp, 1, 0); // dataProgram empieza dormido
    sem_init(&shm->sem_ui, 1, 0); // ui empieza dormido

    /* Cargar indice hash desde archivo — los datos quedan en disco */
    cargar_hash_desde_binario("hash.bin");

    FILE *peliculas = fopen("peliculas.bin", "rb+");
    if (peliculas == NULL) {
        perror("Error al abrir peliculas.bin");
        exit(-1);
    }

    /* Loop principal — sem_wait duerme hasta que ui mande una query */
    int corriendo = 1;

    while (corriendo) {

        sem_wait(&shm->sem_dp);

        switch (shm->query.searchCriteria) {

            case SEARCH: {
                /* Sin filtros: usar buscar_por_nombre (mas eficiente)
                   Con filtros: armar struct Movie y usar buscar_por_filtros */
                if (shm->query.filterYear == -1 &&
                    strcmp(shm->query.filterType,  "N") == 0 &&
                    strcmp(shm->query.filterGenre, "N") == 0) {

                    shm->movie = buscar_por_nombre(shm->query.primaryTitle, peliculas);

                } else {

                    Movie filtros;
                    strcpy(filtros.primaryTitle,  shm->query.primaryTitle);
                    strcpy(filtros.titleType,     shm->query.filterType);
                    filtros.startYear           = shm->query.filterYear;
                    strcpy(filtros.genres,        shm->query.filterGenre);
                    filtros.isAdult             = -1; // no se filtra por estos campos
                    filtros.runtimeMinutes      = -1;
                    strcpy(filtros.originalTitle, "N");

                    shm->movie = buscar_por_filtros(filtros, peliculas);
                }

                /* Indicar a ui si se encontro o no */
                shm->found = (shm->movie.primaryTitle[0] != '\0') ? 1 : 0;

                sem_post(&shm->sem_ui); // despertar a ui con el resultado
                break;
            }

            case ADD_MOVIE: {
                /* ui ya lleno shm->movie — insertar en .bin y actualizar hash */
                insertar_pelicula_en_binario(shm->movie, "peliculas.bin");
                shm->found = 1;
                sem_post(&shm->sem_ui);
                break;
            }

            case EXIT:
                corriendo = 0;
                break;
        }
    }

    /* Limpieza — destruir semaforos, desconectar y eliminar shm del kernel */
    fclose(peliculas);
    sem_destroy(&shm->sem_dp);
    sem_destroy(&shm->sem_ui);
    shmdt(shm);
    shmctl(shm_id, IPC_RMID, NULL); // sin esto el segmento queda en el sistema
    return 0;
}



