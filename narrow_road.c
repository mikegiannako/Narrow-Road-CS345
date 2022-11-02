#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>
#include "random.h"

typedef enum {RED, BLUE} Color;
typedef enum {EAST, WEST} Direction;

typedef struct Pedestrian{
    int id;                 // Pedestrian ID
    Color color;            // Either Blue or Red
    Direction direction;    // Either East or West
    //pthread_t thread;       // The thread that represents this pedestrian
}* Pedestrian_t;

// Represents act of a pedestrian taking a step
// until they reach the other side of the road
void *cross_road(void *arg);
// Prints a pedestrian's information
void print_pedestrian(Pedestrian_t pedestrian);
// Prints the current state of the road
void print_road(int capacity, Pedestrian_t pedestrians[]);
// Initializes the road with pedestrians and returns how many blue 
// pedestrians are on the road
int init_road(int capacity, Pedestrian_t pedestrians[]);


int main(int argc, char *argv[]){
    // Checks if the number of command line arguments is correct
    if(argc != 3){
        printf("Usage: %s -p <number of pedestrians>\n", argv[0]);
        return 1;
    }

    // Checks if the number of pedestrians is valid
    int num_pedestrians = atoi(argv[2]);
    if(num_pedestrians < 1){
        printf("Invalid number of pedestrians: %d\n", num_pedestrians);
        return 1;
    }

    // Create the road full of pedestrians
    Pedestrian_t* road = calloc(num_pedestrians, sizeof(Pedestrian_t));
    // Also keeps the count of blue and red pedestrians
    int blue_count = init_road(num_pedestrians, road);
    int red_count = num_pedestrians - blue_count;

    // Prints the road's initial state
    print_road(num_pedestrians, road);

    

    return 0;
}

// void* cross_road(void *arg){
//     Pedestrian_t pedestrian = (Pedestrian_t)arg;
// }

// Prints a pedestrian's information
void print_pedestrian(Pedestrian_t pedestrian){
    if(pedestrian == NULL){
        return;
    }

    if(pedestrian->color == BLUE){
        if(pedestrian->direction == EAST){
            printf("\033[1;34m %d > \033[0m", pedestrian->id);
            return;
        }
        
        printf("\033[1;34m < %d \033[0m", pedestrian->id);

        return;
    } 

    if(pedestrian->direction == EAST){
        printf("\033[1;31m %d > \033[0m", pedestrian->id);
        return;
    }

    printf("\033[1;31m < %d \033[0m", pedestrian->id);
    return;
}

// Prints the current state of the road
void print_road(int capacity, Pedestrian_t road[]){
    // For every spot on the road prints the pedestrian that is there else print a space
    for(int i = 0; i < capacity; i++){
        printf("||");
        if(road[i] == NULL){
            printf(" "); 
            continue;
        }

        print_pedestrian(road[i]);
    }
    printf("||");
    return;
}

// Initializes the road with pedestrians and returns how many blue 
// pedestrians are on the road
int init_road(int capacity, Pedestrian_t road[]){
    int east_pointer = 0;
    int west_pointer = capacity - 1;
    int blue_count = 0;

    // Creates the pedestrians and place them on the opposite side of
    // the road based on their direction
    for(int i = 0; i < capacity; i++){
        Pedestrian_t pedestrian = malloc(sizeof(struct Pedestrian));
        pedestrian->id = i + 1;
        pedestrian->color = random(0, 1) ? BLUE : RED;

        pedestrian->direction = random(0, 1) ? EAST : WEST;

        if(pedestrian->direction == EAST){
            road[east_pointer] = pedestrian;
            east_pointer++;
            continue;
        }

        road[west_pointer] = pedestrian;
        west_pointer--;
    }
    return;
}