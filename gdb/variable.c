/*************************************************************************
	> File Name: variable.c
	> Author: 
	> Mail: 
	> Created Time: 2016年03月18日 星期五 08时50分18秒
 ************************************************************************/

#include<stdio.h>

int test(int *p)
{
    int i = 10;
    if(NULL == p)
    return -1;

    return 0;
}

int main()
{
    int *p = NULL;
    test(p);
    return 0;
}
