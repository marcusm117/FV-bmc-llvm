#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int n;
int result;

int main() {
    n = 5;
    result = 1;
    result = 2;

    while (n > 0) result *= n--;

    printf("%d\n", result);
    //assert(a == b);
    //printf("Done\n");
}
