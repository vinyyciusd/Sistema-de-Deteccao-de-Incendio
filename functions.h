#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

#define SIZE 30

typedef struct {
    int x, y;
    char state;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sensor_t;

void initialize_forest(char forest[SIZE][SIZE], sensor_t sensors[SIZE][SIZE]);
void sensor_thread(voidarg);
void fire_generator_thread(voidarg);
void control_center_thread(voidarg);
void print_forest(char forest[SIZE][SIZE]);
void propagate_fire(sensor_t sensor, sensor_t sensors[SIZE][SIZE], char forest[SIZE][SIZE]);
void extinguish_fire(sensor_tsensor, char forest[SIZE][SIZE]);

#endif