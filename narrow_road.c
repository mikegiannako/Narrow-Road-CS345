#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>
#include "random.h"

typedef enum {RED, BLUE} Color;
typedef enum {EAST, WEST} Direction;
typedef enum {ROAD, PAVEMENT} State;

typedef struct Pedestrian{
    int id;                 // Pedestrian ID
    Color color;            // Either Blue or Red
    Direction direction;    // Either East or West
    State state;            // Either Road or Pavement
    //pthread_t thread;       // The thread that represents this pedestrian
}* Pedestrian_t;

// Represents act of a pedestrian taking a step
// until they reach the other side of the road
void *cross_road(void *arg);
// Prints a pedestrian's information
void print_pedestrian(Pedestrian_t pedestrian);
// Prints the current state of the road
void print_road(int capacity, Pedestrian_t pedestrians[]);
// Prints dividing line between road states
void print_divider(int capacity);
// Initializes the road with pedestrians and returns how many blue 
// pedestrians are on the road
int init_road(int capacity, Pedestrian_t pedestrians[]);
// Creates a copy of the road without the pedestrians of the given color and direction
// and returns it. Caller is responsible for freeing the memory allocated for the copy
Pedestrian_t *copy_road(int capacity, Pedestrian_t pedestrians[], Color color, Direction direction);
// Removes all pedestrians except those of the given color and direction from the road
void remove_pedestrians(int capacity, Pedestrian_t pedestrians[], Color color, Direction direction);
// Finds the dominant direction of the pedestrians of the chosen color
Direction find_dominant_direction(int capacity, Pedestrian_t pedestrians[], Color color);


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
    Color dominant_color = blue_count > num_pedestrians / 2 ? BLUE : RED;

    // Prints the road's initial state
    print_road(num_pedestrians, road);
    puts("\n");

    // After initializing the road and finding the dominant color we need to find the 
    // dominant direction to start the simulation
    Direction dominant_direction = find_dominant_direction(num_pedestrians, road, BLUE);

    // The simulation starts here

    // Makes a copy of the road (which will represent the pavement) without the pedestrians of the 
    // dominant color and direction. We have to free the memory allocated for the copy later
    Pedestrian_t *pavement = copy_road(num_pedestrians, road, dominant_color, dominant_direction);

    // Removes all other pedestrians that aren't of the dominant color and direction from the road
    remove_pedestrians(num_pedestrians, road, dominant_color, dominant_direction);

    // Prints the road's state after removing the pedestrians
    print_road(num_pedestrians, road);
    print_divider(num_pedestrians);
    print_road(num_pedestrians, pavement);
    
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
            printf("\033[1;34m %02d > \033[0m", pedestrian->id);
            return;
        }
        
        printf("\033[1;34m < %02d \033[0m", pedestrian->id);

        return;
    } 

    if(pedestrian->direction == EAST){
        printf("\033[1;31m %02d > \033[0m", pedestrian->id);
        return;
    }

    printf("\033[1;31m < %02d \033[0m", pedestrian->id);
    return;
}

// Prints the current state of the road
void print_road(int capacity, Pedestrian_t road[]){
    // For every spot on the road prints the pedestrian that is there else print a space
    for(int i = 0; i < capacity; i++){
        printf("||");
        if(road[i] == NULL){
            printf("      "); 
            continue;
        }

        print_pedestrian(road[i]);
    }
    printf("||\n");
    return;
}

// Prints dividing line between road states
void print_divider(int capacity){
    for(int i = 0; i < capacity; i++){
        printf("________");
    }
    printf("__\n");
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
        // Keeps track of how many blue pedestrians are on the road
        if(pedestrian->color == BLUE) blue_count++;
        pedestrian->direction = random(0, 1) ? EAST : WEST;
        pedestrian->state = ROAD;

        if(pedestrian->direction == EAST){
            road[east_pointer] = pedestrian;
            east_pointer++;
            continue;
        }

        road[west_pointer] = pedestrian;
        west_pointer--;
    }

    return blue_count;
}

// Creates a copy of the road without the pedestrians of the given color and direction
// and returns it. Caller is responsible for freeing the memory allocated for the copy
Pedestrian_t *copy_road(int capacity, Pedestrian_t pedestrians[], Color color, Direction direction){
    Pedestrian_t *copy = calloc(capacity, sizeof(Pedestrian_t));
    for(int i = 0; i < capacity; i++){
        if(pedestrians[i]->color != color || pedestrians[i]->direction != direction){
            copy[i] = pedestrians[i];
            copy[i]->state = PAVEMENT;
        }
    }
    return copy;
}

// Removes all pedestrians except those of the given color and direction from the road
void remove_pedestrians(int capacity, Pedestrian_t pedestrians[], Color color, Direction direction){
    for(int i = 0; i < capacity; i++){
        if(pedestrians[i]->color != color || pedestrians[i]->direction != direction){
            pedestrians[i] = NULL;
        }
    }
}

// Finds the dominant direction of the pedestrians of the chosen color
Direction find_dominant_direction(int capacity, Pedestrian_t pedestrians[], Color color){
    int east_count = 0;
    int west_count = 0;

    for(int i = 0; i < capacity; i++){
        if(pedestrians[i]->color != color){
            continue;
        }

        if(pedestrians[i]->direction == EAST){
            east_count++;
            continue;
        }

        west_count++;
    }

    return east_count > west_count ? EAST : WEST;
}