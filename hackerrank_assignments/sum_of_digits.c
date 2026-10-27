#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int find_sum_of_digits(int number)
{
    int sum_of_digits = 0;
    while(number > 0)
    {
        sum_of_digits += (number % 10);
        number /= 10;
    }
    return sum_of_digits;
}

int main() {
	
    int five_digit_integer;
    scanf("%d", &five_digit_integer);
    printf("%d",find_sum_of_digits(five_digit_integer));
    return 0;
}
