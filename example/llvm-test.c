#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int foo() {
    return 10;
}

int main() {
    const int a = 5;
    const int b = foo();
    assert(a > b);
    printf("Done\n");
}
