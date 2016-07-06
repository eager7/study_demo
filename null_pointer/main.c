/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: 2015年11月18日 星期三 10时25分52秒
 ************************************************************************/

#include<stdio.h>

void test(int *p)
{
    if (p == NULL)
    {
        printf("null poniter\n");
    }
        return;
}
int main(void)
{
    printf("hello test\n");
    int *p = NULL;

    test(p);
    return 0;
}
