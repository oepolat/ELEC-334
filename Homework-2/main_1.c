#include <stdio.h>

#include "myrand.h"

int main()
{
    set_seed(1);
    
    printf("random number: %d", randomize(1, 15));
    return 0;
}