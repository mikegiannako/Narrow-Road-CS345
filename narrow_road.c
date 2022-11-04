#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "random.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum {RED, BLUE} Color;
typedef enum {EAST = 1, WEST = -1} Direction;
typedef enum {ROAD, PAVEMENT, FINISHED} State;

typedef struct Pedestrian{
    int id;                 // Pedestrian ID
    int index;              // Pedestrian index on the road
    Color color;            // Either Blue or Red
    Direction direction;    // Either East or West
    State state;            // Either Road, Pavement, or Finished
}* Pedestrian_t;

typedef struct Arg{
    Pedestrian_t pedestrian;
    Pedestrian_t *road;
    int road_length;
}* Arg_t;

int finished = 0;
Pedestrian_t *pavement;

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
// Changes the state of all pedestrians in the given road to the given state
void change_state(int capacity, Pedestrian_t pedestrians[], State state);
// Returns if the given road is empty or not
int is_empty(int capacity, Pedestrian_t pedestrians[]);
// Prints the state of the road and the pavement
void print_road_state(int capacity, Pedestrian_t road[], Pedestrian_t pavement[], char* message);


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
    Pedestrian_t *road = calloc(num_pedestrians, sizeof(Pedestrian_t));
    // Also keeps the count of blue and red pedestrians
    int blue_count = init_road(num_pedestrians, road);
    Color dominant_color = blue_count > num_pedestrians / 2 ? BLUE : RED;

    // Prints the road's initial state
    puts("Initial state of the road:");
    print_road(num_pedestrians, road);

    // Wait for 3 seconds
    sleep(3);

    // After initializing the road and finding the dominant color we need to find the
    // dominant direction to start the simulation
    Direction dominant_direction = find_dominant_direction(num_pedestrians, road, dominant_color);

    // The simulation starts here

    // Create threads for each pedestrian
    pthread_t threads[num_pedestrians];
    for(int i = 0; i < num_pedestrians; i++){
        // Create the argument for the thread
        Arg_t arg = malloc(sizeof(struct Arg));
        arg->pedestrian = road[i];
        arg->road = road;
        arg->road_length = num_pedestrians;

        // Create the thread
        pthread_create(&threads[i], NULL, cross_road, arg);
    }

    // Makes a copy of the road (which will represent the pavement) without the pedestrians of the
    // dominant color and direction. We have to free the memory allocated for the copy later
    pavement = copy_road(num_pedestrians, road, dominant_color, dominant_direction);


    // Removes all other pedestrians that aren't of the dominant color and direction from the road
    remove_pedestrians(num_pedestrians, road, dominant_color, dominant_direction);

    print_road_state(num_pedestrians, road, pavement,
     "State before the first step of the simulation starts:");
    sleep(2);

    // As soon as we change the state to ROAD the pedestrians will start moving
    change_state(num_pedestrians, road, ROAD);

    // Waiting for the pedestrians to cross the road
    while(!is_empty(num_pedestrians, road));

    // After the first step we need the pedestrians of the dominant color and the opposite direction
    // to move as well while the others wait at the pavement

    // Make the road a copy of the pavement
    for(int i = 0; i < num_pedestrians; i++) road[i] = pavement[i];

    free(pavement);
    pavement = copy_road(num_pedestrians, road, dominant_color, dominant_direction * -1);
    remove_pedestrians(num_pedestrians, road, dominant_color, dominant_direction * -1);

    print_road_state(num_pedestrians, road, pavement,
     "State before the second step of the simulation starts:");
    sleep(2);

    change_state(num_pedestrians, road, ROAD);

    while(!is_empty(num_pedestrians, road));

    for(int i = 0; i < num_pedestrians; i++) road[i] = pavement[i];
    free(pavement);

    dominant_color = dominant_color == BLUE ? RED : BLUE;
    dominant_direction = find_dominant_direction(num_pedestrians, road, dominant_color);

    pavement = copy_road(num_pedestrians, road, dominant_color, dominant_direction);
    remove_pedestrians(num_pedestrians, road, dominant_color, dominant_direction);

    print_road_state(num_pedestrians, road, pavement,
     "State before the thrid step of the simulation starts:");
    sleep(2);

    change_state(num_pedestrians, road, ROAD);

    while(!is_empty(num_pedestrians, road));

    for(int i = 0; i < num_pedestrians; i++) road[i] = pavement[i];
    free(pavement);
    pavement = copy_road(num_pedestrians, road, dominant_color, dominant_direction * -1);
    remove_pedestrians(num_pedestrians, road, dominant_color, dominant_direction * -1);

    print_road_state(num_pedestrians, road, pavement,
     "State before the last step of the simulation starts:");
    sleep(2);

    change_state(num_pedestrians, road, ROAD);

    while(!is_empty(num_pedestrians, road));

    finished = 1;

    // Wait for all the threads to finish
    for(int i = 0; i < num_pedestrians; i++) pthread_join(threads[i], NULL);

    return 0;
}

void *cross_road(void *arg){
    Arg_t args = (Arg_t)arg;
    Pedestrian_t pedestrian = args->pedestrian;
    Pedestrian_t *road = args->road;
    int road_length = args->road_length;

    while(pedestrian->state != FINISHED){
        // When the pedestrian is on the road he has to move until he reaches the other side
        // checking if there is a pedestrian in front of him
        if(pedestrian->state == ROAD){
            // If the next position is the end of the road the pedestrian has reached the other side
            if(pedestrian->index + pedestrian->direction == road_length
                || pedestrian->index + pedestrian->direction == -1){

                pthread_mutex_lock(&mutex);
                pedestrian->state = FINISHED;
                road[pedestrian->index] = NULL;
                print_road_state(road_length, road, pavement, NULL);
                pthread_mutex_unlock(&mutex);
                continue;
            }

            // If there is a pedestrian in front of him he has to wait
            if(road[pedestrian->index + pedestrian->direction] != NULL) continue;

            // If there is no pedestrian in front of him he can move
            pthread_mutex_lock(&mutex);
            pedestrian->index += pedestrian->direction;
            road[pedestrian->index] = pedestrian;
            road[pedestrian->index - pedestrian->direction] = NULL;
            print_road_state(road_length, road, pavement, NULL);
            pthread_mutex_unlock(&mutex);
        }
    }

    // After the pedestrian has reached the other side of the road we
    // free the memory allocated for him
    free(pedestrian);

    // We don't free the road because it is common for all pedestrians
    free(arg);

    while(!finished);
    sleep(1);

    return NULL;
}

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
        pedestrian->color = randomc(0, 1) ? BLUE : RED;
        // Keeps track of how many blue pedestrians are on the road
        if(pedestrian->color == BLUE) blue_count++;
        pedestrian->direction = randomc(0, 1) ? EAST : WEST;
        pedestrian->state = PAVEMENT;


        if(pedestrian->direction == EAST){
            road[east_pointer] = pedestrian;
            pedestrian->index = east_pointer;
            east_pointer++;
            continue;
        }

        road[west_pointer] = pedestrian;
        pedestrian->index = west_pointer;
        west_pointer--;
    }

    return blue_count;
}

// Creates a copy of the road without the pedestrians of the given color and direction
// and returns it. Caller is responsible for freeing the memory allocated for the copy
Pedestrian_t *copy_road(int capacity, Pedestrian_t pedestrians[], Color color, Direction direction){
    Pedestrian_t *copy = calloc(capacity, sizeof(Pedestrian_t));
    for(int i = 0; i < capacity; i++){
        if(!pedestrians[i] || pedestrians[i]->color != color || pedestrians[i]->direction != direction){
            copy[i] = pedestrians[i];
        }
    }
    return copy;
}

// Removes all pedestrians except those of the given color and direction from the road
void remove_pedestrians(int capacity, Pedestrian_t pedestrians[], Color color, Direction direction){
    for(int i = 0; i < capacity; i++){
        if(!pedestrians[i]) continue;
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
        if(!pedestrians[i]) continue;
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

// Changes the state of all pedestrians in the given road to the given state
void change_state(int capacity, Pedestrian_t pedestrians[], State state){
    for(int i = 0; i < capacity; i++){
        if(pedestrians[i] != NULL){
            pedestrians[i]->state = state;
        }
    }
}

// Returns if the given road is empty or not
int is_empty(int capacity, Pedestrian_t pedestrians[]){
    for(int i = 0; i < capacity; i++){
        if(pedestrians[i] != NULL){
            return 0;
        }
    }
    return 1;
}

// Prints the state of the road and the pavement
void print_road_state(int capacity, Pedestrian_t road[], Pedestrian_t pavement[], char* message){
    // Clear the screen
    // printf("\033[H\033[J");

    if(message) puts(message);

    print_road(capacity, road);
    print_divider(capacity);
    print_road(capacity, pavement);
    puts("\n");
}