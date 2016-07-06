#include <stdio.h>

int main(int argc, char *argv[])
{
    unsigned int i = 0;
    unsigned int j = 1;
    //printf("i=%d, j=%d\n", --i, j--);

    while(--i){
        printf("Test:%d\n", i);
        sleep(1);
    }
    return 0;
}
