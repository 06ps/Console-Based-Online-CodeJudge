#include <stdio.h>

void main(int argc, char const *argv[])
{
    int num1, num2;
    scanf("%d %d", &num1, &num2);
    float result = num1 / (num2 * 1.0);
    if (num2 == 0)
        printf("MATH ERROR!!\n");
    else
        printf("%.2f\n", result);
}
