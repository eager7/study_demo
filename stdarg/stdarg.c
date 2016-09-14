#include <stdio.h>
#include <stdarg.h>

double average(int number, ...)
{
    double total = 0;
    va_list ap;

    va_start(ap, number);
    for(int j = 0; j < number; j++){
        total += va_arg(ap, double);
    }
    va_end(ap);

    return total/number;
}


void main()
{
    printf("test va arg\n");
    printf("(1+2+3+4+5) average = %f\n", average(5, 1.0,2.0,3.0,4.0,5.0));
}
