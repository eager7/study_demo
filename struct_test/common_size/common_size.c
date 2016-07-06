#include <stdio.h>
#include "common.h"

int main(void)
{
    printf("common_size test\n");
	printf("the size is:%d\n", sizeof(TL_CmdDataS));//472
	printf("the size is:%d\n", sizeof(TL_UintPropDataS));//1340
	printf("the size is:%d\n", sizeof(TL_UintPropDataS2));//5396
	printf("the size is:%d\n", sizeof(TL_Properties));//868
	printf("the size is:%d\n", sizeof(TL_GroupPropDataS));//43876
	printf("the size is:%d\n", sizeof(TL_CommuPacketDatas));//43876
    return 0;
}
