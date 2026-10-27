#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

void print_pattern(int n)
{
    int s = 0, e = n * 2 - 1;
    for(int i = 0; i < n * 2 - 1; i++)
    {
        int k = n;
        for(int j = 0; j < n * 2 - 1; j++)
        {
           if(j >= s && j < e)
           {
             printf("%d ", k);
           } 
           else if(j < s)
           {
             printf("%d ",k--);
           }
           else if(j >= e)
           {
            printf("%d ",++k);
           }
        }
        printf("\n");
        if(i >= n - 1)
        {
            s--;
            e++;
        }
        else
        {
            s++;
            e--;
        }
    }
}

int main() 
{
    int n;
    scanf("%d", &n);
  	print_pattern(n);
    return 0;
}
