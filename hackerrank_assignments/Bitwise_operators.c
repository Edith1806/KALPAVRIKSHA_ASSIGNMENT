#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
//Complete the following function.


void calculate_the_maximum(int n, int k) {
   int max_and, max_or, max_xor;
   max_and = max_or = max_xor = 0;
  for(int current_num = 1; current_num <= n; current_num++)
  {
    for(int compare_num = current_num + 1; compare_num <= n; compare_num++)
    {
        int current_and = current_num & compare_num;
        int current_or = current_num | compare_num;
        int current_xor = current_num ^ compare_num;
        if(current_and < k && current_and > max_and)
            max_and = current_and;
        if(current_or < k && current_or > max_or)
            max_or = current_or;
        if(current_xor < k && current_xor > max_xor)
            max_xor = current_xor;
    }
  }
  printf("%d\n%d\n%d",max_and,max_or,max_xor);
}

int main() {
    int n, k;
  
    scanf("%d %d", &n, &k);
    calculate_the_maximum(n, k);
 
    return 0;
}
