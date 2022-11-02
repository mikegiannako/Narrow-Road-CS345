#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "random.h"

typedef enum {RED, BLUE} Color;
typedef enum {EAST, WEST} Direction;

typedef struct {
    int id;                 // Pedestrian ID
    Color color;            // Either Blue or Red
    Direction direction;    // Either East or West
    pthread_t thread;       // The thread that represents this pedestrian
} Pedestrian;

// Function that represents act of a pedestrian taking a step
// until they reach the other side of the road
void *cross_road(void *arg);

// Command line arguments include the number of -p followed by the number of pedestrians
int main(int argc, char *argv[]){
    // Check if the number of command line arguments is correct
    if(argc != 3){
        printf("Usage: %s -p <number of pedestrians>\n", argv[0]);
        return 1;
    }

    // Check if the number of pedestrians is valid
    int num_pedestrians = atoi(argv[2]);
    if(num_pedestrians < 1){
        printf("Invalid number of pedestrians: %d\n", num_pedestrians);
        return 1;
    }

    // Create an array of pedestrians
    Pedestrian pedestrians[num_pedestrians];

    // Create the pedestrians


    return 0;
}

void* cross_road(void *arg){
    Pedestrian *pedestrian = (Pedestrian *)arg;
}



