#include <stdio.h>

#include "myrand.h"

static const int a = 1103515245;
static const int c = 1234567;
static const int m = 2147483647;

static int seed = 1;
static int random = 0;

void set_seed(int s)
{
    seed = s;
}

int randomize(int min, int max)
{
    //uses a random number generating algorithm
    seed = ((a * seed) + c) % m;

    //normalizes values for the size
    random = (seed % (max));

    //normalizes the values to fit the minimum values and makes the negative values from the remainder operator positive values
    return ((random) < 0 ? (0 - (random)) + min : (random) + min);
}