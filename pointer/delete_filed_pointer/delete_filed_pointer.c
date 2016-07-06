#include <stdio.h>
#include <malloc.h>

int main()
{
    char *p = (char *)malloc(10);
    sprintf(p, "%s", "1234567890");
    printf("%s\n", p);
    free(p);
    free(p);

    return 0;
}
