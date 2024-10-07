#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

sensor_t sensors[SIZE][SIZE];
char forest[SIZE][SIZE];
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t control_center_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t control_center_cond = PTHREAD_COND_INITIALIZER;

int fire_detected = 0;
int fire_x = -1, fire_y = -1;

void initialize_forest(char forest[SIZE][SIZE], sensor_t sensors[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            forest[i][j] = '-';  // Área livre
            sensors[i][j].x = i;
            sensors[i][j].y = j;
            sensors[i][j].state = '-';
            pthread_mutex_init(&sensors[i][j].mutex, NULL);
            pthread_cond_init(&sensors[i][j].cond, NULL);
        }
    }

    // Posicionar sensores aleatoriamente na floresta
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (rand() % 5 == 0) {  // 20% das células terão sensores
                forest[i][j] = 'T';
                sensors[i][j].state = 'T';
            }
        }
    }
}

void *sensor_thread(void *arg) {
    sensor_t *sensor = (sensor_t *)arg;

    while (1) {
        pthread_mutex_lock(&sensor->mutex);

        if (sensor->state == '@') {
            // Notifica a central de controle se o sensor estiver na borda
            if (sensor->x == 0 || sensor->x == SIZE - 1 || sensor->y == 0 || sensor->y == SIZE - 1) {
                pthread_mutex_lock(&control_center_mutex);
                fire_detected = 1;
                fire_x = sensor->x;
                fire_y = sensor->y;
                pthread_cond_signal(&control_center_cond);
                pthread_mutex_unlock(&control_center_mutex);
            }

            // Propagar incêndio para os vizinhos
            propagate_fire(sensor, sensors, forest);

            // Extinguir o fogo após um tempo
            sleep(2);
            extinguish_fire(sensor, forest);
        }

        pthread_mutex_unlock(&sensor->mutex);
        sleep(1);
    }
    return NULL;
}

void propagate_fire(sensor_t *sensor, sensor_t sensors[SIZE][SIZE], char forest[SIZE][SIZE]) {
    int x = sensor->x;
    int y = sensor->y;

    // Propagar o fogo para os sensores vizinhos
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if ((dx != 0 || dy != 0) && x + dx >= 0 && x + dx < SIZE && y + dy >= 0 && y + dy < SIZE) {
                pthread_mutex_lock(&sensors[x + dx][y + dy].mutex);
                if (sensors[x + dx][y + dy].state == 'T') {
                    forest[x + dx][y + dy] = '@';
                    sensors[x + dx][y + dy].state = '@';
                }
                pthread_mutex_unlock(&sensors[x + dx][y + dy].mutex);
            }
        }
    }
}

void extinguish_fire(sensor_t *sensor, char forest[SIZE][SIZE]) {
    int x = sensor->x;
    int y = sensor->y;

    forest[x][y] = '/';  // Célula queimada
    sensor->state = '/'; // Atualizar estado do sensor para queimado
}

void *fire_generator_thread(void *arg) {
    while (1) {
        int x = rand() % SIZE;
        int y = rand() % SIZE;

        pthread_mutex_lock(&sensors[x][y].mutex);
        if (forest[x][y] == 'T') {
            forest[x][y] = '@';
            sensors[x][y].state = '@';
        }
        pthread_mutex_unlock(&sensors[x][y].mutex);

        sleep(3);  // Gerar incêndio a cada 3 segundos
    }
    return NULL;
}

void *control_center_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&control_center_mutex);
        while (!fire_detected) {
            pthread_cond_wait(&control_center_cond, &control_center_mutex);
        }

        printf("Central de controle: Incêndio detectado em (%d, %d)\n", fire_x, fire_y);
        fire_detected = 0;  // Resetar a condição de incêndio

        pthread_mutex_unlock(&control_center_mutex);

        // Aqui a central de controle poderia tomar decisões adicionais, como despachar uma equipe de combate ao incêndio
        sleep(1);  // Simulação de tempo de resposta
    }
    return NULL;
}

void print_forest(char forest[SIZE][SIZE]) {
    pthread_mutex_lock(&print_mutex);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", forest[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    pthread_mutex_unlock(&print_mutex);
}