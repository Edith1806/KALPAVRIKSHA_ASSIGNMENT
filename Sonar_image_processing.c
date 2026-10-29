#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<string.h>

void free_matrix(int dimension, int ***matrix)
{
    if(matrix == NULL) return;
    for(int row = 0; row < dimension; row++)
    {
        if(*(matrix + row) != NULL)
        {
        for(int col = 0; col < dimension; col++)
        {
            free(*(*(matrix + row) + col));
        }
        free(*(matrix + row));
        }
    }
    free(matrix);
}

bool get_matrix_data(int dimension, int ***matrix)
{
    
    for(int row = 0; row < dimension; row++)
    {
        *(matrix + row) = calloc(dimension, sizeof **matrix);
        if(*(matrix + row) == NULL)
        {
            printf("System error: Memory allocation failed.\n");
            free_matrix(dimension, matrix);
            return false;
        }
        for(int col = 0; col < dimension; col++)
        {
            *(*(matrix + row) + col) = malloc(sizeof ***matrix);
            if(*(*(matrix + row) + col) == NULL)
            {
                printf("System error: Memory allocation failed!\n");
                free_matrix(dimension, matrix);
                return false;
            }
            *(*(*(matrix + row) + col)) = rand() % 256; 
        }      
    }
    return true;
}

void reverse_row(int **matrix_row,int dimension)
{
    int start_idx = 0;
    int end_idx = dimension - 1;
    while(start_idx < end_idx)
    {
        int *temp = *(matrix_row + start_idx);
        *(matrix_row + start_idx) = *(matrix_row + end_idx);
        *(matrix_row + end_idx) = temp;
        start_idx++;
        end_idx--;
    }
}

void rotate_matrix(int dimension, int ***matrix)
{
    for(int row = 0; row < dimension; row++)
    {
        for(int col = 0; col < row; col++)
        {
            int *temp = *(*(matrix + row) + col);
            *(*(matrix + row) + col) = *(*(matrix + col) + row);
            *(*(matrix + col) + row) = temp;
        }
    }

    for(int row = 0; row < dimension; row++)
    {
        reverse_row(*(matrix + row), dimension);
    }
}

void print_matrix(char *prompt, int dimension, int ***matrix)
{
    printf("%s\n", prompt);
    for(int row = 0; row < dimension; row++)
    {
        for(int col = 0; col < dimension; col++)
        {
            printf("%4d", *(*(*(matrix + row) + col)));
        }
        printf("\n");
    }
    printf("\n\n");
}

bool smooth_matrix(int dimension, int ***matrix)
{
    int *previous = malloc(dimension * sizeof *previous);
    int *current = malloc(dimension * sizeof *current);
    int *next = malloc(dimension * sizeof *next);
    if (previous == NULL || current == NULL || next == NULL)
    {
        free(previous);
        free(current);
        free(next);
        printf("System error: Memory allocation failed!\n");
        return false;
    }
    for (int col = 0; col < dimension; col++)
    {
        *(current + col) = *(*(*(matrix) + col));
    }
    for (int col = 0; col < dimension; col++)
    {
        *(next + col) = *(*(*(matrix + 1) + col));
    }

    for (int row = 0; row < dimension; row++)
    {
        for (int col = 0; col < dimension; col++)
        {
            int sum = 0;
            int count_neighbours = 0;
            for (int row_offset = -1; row_offset <= 1; row_offset++)
            {
                for (int col_offset = -1; col_offset <= 1; col_offset++)
                {
                    int negh_row = row + row_offset;
                    int negh_col = col + col_offset;

                    if (negh_row >= 0 && negh_row < dimension &&
                        negh_col >= 0 && negh_col < dimension)
                    {
                        if (negh_row == row - 1)
                        {
                            sum += *(previous + negh_col);
                        }
                        else if (negh_row == row)
                        {
                            sum += *(current + negh_col);
                        }
                        else
                        {
                            sum += *(next + negh_col);
                        }

                        count_neighbours++;
                    }
                }
            }
            *(*(*(matrix + row) + col)) = sum / count_neighbours;
        }
        if (row < dimension - 1)
        {
            int *temp = previous;
            previous = current;
            current = next;
            next = temp;
            if (row + 2 < dimension)
            {
                for (int col = 0; col < dimension; col++)
                {
                    *(next + col) = *(*(*(matrix + row + 2) + col));
                }
            }
        }
    }
    free(previous);
    free(current);
    free(next);
    return true;
}

int main(void)
{
    int matrix_dimension;
    printf("Enter a number N for N x N matrix: ");
    while(scanf("%d", &matrix_dimension) != 1 || (matrix_dimension < 2 || matrix_dimension > 10))
    {
        printf("Invalid matrix size. Matrix size should be between 2 - 10 (inclusive)\nEnter a number N for N x N matrix: ");
    }
    int ***matrix = calloc(matrix_dimension, sizeof *matrix);
    if(matrix == NULL)
    {
        printf("System error: Memory allocation failed.\n");
        return 1;
    }
    srand((unsigned int)time(NULL));
    if(!get_matrix_data(matrix_dimension, matrix))
    {
        return 1;
    }
    print_matrix("Original Randomly Generated Matrix:", matrix_dimension, matrix);
    rotate_matrix(matrix_dimension, matrix);
    print_matrix("Matrix after 90 degree Clockwise Rotation:", matrix_dimension, matrix);
    if(!smooth_matrix(matrix_dimension, matrix))
    {
        free_matrix(matrix_dimension, matrix);
        return 1;
    }
    print_matrix("Matrix after Applying 3x3 Smoothing Filter:", matrix_dimension, matrix);
    free_matrix(matrix_dimension, matrix);
    return 0;
}
