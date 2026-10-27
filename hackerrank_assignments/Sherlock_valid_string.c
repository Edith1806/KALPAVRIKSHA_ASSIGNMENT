#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();
char* isValid(char* s) {
   int frequency_arr[26] = {0};
    for(int i = 0; s[i] != '\0'; i++)
    {
        frequency_arr[s[i] - 'a']++;
    }
    
    int frequency1 = 0, count1 = 0, frequency2 = 0, count2 = 0;
    for(int i = 0; i < 26; i++)
    {
        if(frequency_arr[i] == 0)
            continue;
        if(frequency1 == 0 || frequency1 == frequency_arr[i])
        {
            frequency1 = frequency_arr[i];
            count1++;
        }
        else if(frequency2 == 0 || frequency2 == frequency_arr[i])
        {
            frequency2 = frequency_arr[i];
            count2++;
        }
        else
        {
            return "NO";
        }
    }
    if(frequency2 == 0)
    {
        return "YES";
    }
    if((frequency1 == 1 && count1 == 1) || (count2 == 1))
    {
        return "YES";
    }
    return "NO";
}


int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    char* s = readline();

    char* result = isValid(s);

    fprintf(fptr, "%s\n", result);

    fclose(fptr);

    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;

    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) {
            break;
        }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') {
            break;
        }

        alloc_length <<= 1;

        data = realloc(data, alloc_length);

        if (!data) {
            data = '\0';

            break;
        }
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';

        data = realloc(data, data_length);

        if (!data) {
            data = '\0';
        }
    } else {
        data = realloc(data, data_length + 1);

        if (!data) {
            data = '\0';
        } else {
            data[data_length] = '\0';
        }
    }

    return data;
}
