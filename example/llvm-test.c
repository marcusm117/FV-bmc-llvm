#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main() {
    int n = 5;
    int result = 1;

    while (n > 0) result *= n--;

    printf("%d\n", result);
    //assert(a == b);
    //printf("Done\n");
}
