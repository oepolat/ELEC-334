#include <stdio.h>

#include "test_random.h"
#include "myrand.h"

void test_random(int data_size)
{
    int counter[15] = {0};
    
    for(int i=0; i<data_size; i++)
    {
        int x = randomize(1, 15);
        counter[x-1]++;
    }

    for(int i=0; i<15; i++)
    {
        printf("%d: %d\n", i+1, counter[i]);
    }
}