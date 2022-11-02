#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Initializes the random number generator only once
void init_random(){
    static int initialized = 0;
    if(!initialized){
        srand(time(NULL));
        initialized = 1;
    }
    return;
}

// Returns a random number between [min, max]
int random(int min, int max){
    init_random();
    return rand() % (max - min + 1) + min;
}



// This function didn't end up being used in the program 
// Because I later realized that the placement of pedestrians
// isn't completely random. Keeping it here for future reference

// Returns a randomized arrary with number from 1 to n
int* random_array(int n){
    init_random();
    // Create an array with numbers from 1 to n
    int* array = malloc(n * sizeof(int));
    for(int i = 0; i < n; i++){
        array[i] = i + 1;
    }

    // Randomize the array
    for(int i = 0; i < n; i++){
        int j = random(0, n - 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }

    return array;
}
