# Narrow-Road-CS345

A simulation of narrow road where (using threads) pedestrians of blue and red shirts want to pass from both ways (Assignment 2 of CS345 at CSD @ UoC)

**To compile everything all you need to type is:** make (all)

## General Concept

We start with a narrow road that only fits one pedestrian that can walk and has a pavement on the side where pedestrians wait
We first have to find the dominant color among shirts of pedestrians that want to cross the road and then amongst them find the
dominant direction of movement. We move everyone else to the side so these pedestrians can cross the road. Then we continue with
those of the same color but different direction and finally we repeat for the pedestrians of that wear shirts of the opposite color

#### random.c

This file contains the **randomc function** that generates a random number from [min,max]. There is another function that was
supposed to be the reason this file was made but I later realized that the road isn't initialized in a fully random way.


#### narrow_road.c

This file contains everything else. There are lots of **functions with comments explaining** each one. The main functions is
also there.

### Known issues

For unexpected reasons some times when the last pedestrians crosses the road during the 3rd stage some **pedestrians with
weird IDs may appear at the pavement** for one iteration.

### Future improvements

Perhaps not halting the threads until each one of them is finished

### Knowledge acquired

This assignement introduced me to the following concepts:
- Working with **threads**
  - pthread_create()
  - pthread_join()
  - asynchronous access to shared memory
- Working with mutexes
  - pthread_mutex_init()
  - pthread_mutex_lock()
  - pthread_mutex_unlock()
  - **Critical code sections** 
  - **Deadlock** detection
 
