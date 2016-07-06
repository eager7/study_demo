#include <stdio.h>
#include <string.h>

void main()
{
    printf("null pointer copy test\n");
    int *p = NULL;
    int a = 0;
    memcpy(&a, p, sizeof(a));
    printf("end test\n");
}
