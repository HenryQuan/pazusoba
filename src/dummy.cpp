#include <stdio.h>
#include "pazusoba.h"

int main()
{
    int a = 0;
    const unsigned char *hello = (const unsigned char *)"Hello World";
    printf("Hello, CMake. Hash is %u\n", pazusoba::hash::djb2_hash(hello));
    return 0;
}
