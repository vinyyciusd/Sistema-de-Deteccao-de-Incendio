#include "functions.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Inicializa a floresta e os sensores
    initialize_forest(forest, sensors);

    // Criação das threads dos sensores
    pthread_t sensor_threads[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (sensors[i][j].state == 'T') {
                pthread_create(&sensor_threads[i][j], NULL, sensor_thread, &sensors[i][j]);
            }
        }
    }

    // Criação da thread geradora de incêndios
    pthread_t fire_thread;
    pthread_create(&fire_thread, NULL, fire_generator_thread, NULL);

    // Criação da thread da central de controle
    pthread_t control_thread;
    pthread_create(&control_thread, NULL, control_center_thread, NULL);

    // Loop principal que exibe a floresta
    while (1) {
        print_forest(forest);
        sleep(1);  // Atualiza a visualização da floresta a cada 1 segundo
    }

    return 0;
}