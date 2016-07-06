#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	printf("read and write poniter test\n");

	char *p = (char*)malloc(10);
	if(NULL == p)
	{
		printf("error\n");
		exit(1);
	}

	memset(p, 0, 10);
	sprintf(p, "1234512");
	printf("&p:%p\n", p);
	printf("p:%s\n", p);
	free(p);

	//printf("&p:%p\n", p);
	//sprintf(p, "12345123456789");
	//printf("p:%s\n", p);
	
	//char *q;
	//printf("q:%p\n", q);
	//sprintf(q, "test");
	
	return 0;
}
