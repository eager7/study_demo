/*************************************************************************
	> File Name: vsnprintf.c
	> Author: 
	> Mail: 
	> Created Time: 2015年11月18日 星期三 19时55分46秒
 ************************************************************************/

#include<stdio.h>
#include <stdarg.h>

int my_printf(char *buf, size_t buflen, const char *format, va_list ap)
{
    if(!buflen)
        return -1;

    int r = 0;
    r = vsnprintf(buf, buflen, format, ap);
    buf[buflen-1] = '\0';
    return r;
}

int main(int argc, char *argv[])
{
    char *str = "Hello World!";

    //``my_printf()
    return 0;
}
