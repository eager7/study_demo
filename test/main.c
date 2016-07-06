#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("sizeof(void *) = %d\n",sizeof(void *));
	printf("num = %d\n",0%255);
    fprintf(stderr, "Hello World\n");
	return 0;
}
