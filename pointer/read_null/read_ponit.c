#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	printf("read poniter test\n");
#if 0	
	int *p = 0x12345;
	printf( " the ponit address is:%p\n ", p );
	printf( " the ponit data is:%x\n ", *p );
#endif	
	int *p = ( int* )malloc( sizeof( int ) );
	printf( " the ponit address is:%p\n ", p );
	printf( " the ponit data is:%x\n ", *p );
	free( p );
	
	printf( " the ponit address is:%p\n ", p );
	printf( " the ponit data is:%x\n ", *p );
	return 0;
}
