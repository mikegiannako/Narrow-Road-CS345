#include "random.h"
#include <stdio.h>
#include <time.h>

// Fucntion that returns a random number between [min, max]
int random(int min, int max){
    // Seed the random number generator
    srand(time(NULL));
    return rand() % (max - min + 1) + min;
}