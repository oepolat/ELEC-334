#include <stdio.h>

#include "myrand.h"
#include "test_random.h"

int main()
{
    set_seed(13);

    test_random(150000);

    return 0;
}