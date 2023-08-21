#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 20000

int **randomMatrix()
{
    int **matrix = (int **)malloc(SIZE * sizeof(int *)); // Allocate memory for all rows

    for (int i = 0; i < SIZE; i++)
    {
        matrix[i] = (int *)malloc(SIZE * sizeof(int)); // Allocate memory for each row

        for (int j = 0; j < SIZE; j++)
        {
            matrix[i][j] = rand() % 100;
        }
    }

    return matrix;
}

void printMatrix(int **matrix)
{
    for (int i = 0; i < SIZE; ++i)
    {
        printf("\n");
        for (int j = 0; j < SIZE; ++j)
        {
            printf("%d   \t", matrix[i][j]);
        }
    }
}

int main()
{
    srand(time(0));
    int **matrixSum;
    int **matrix1 = randomMatrix();
    int i = 0, j = 0;
    // printf("\nMatrix 1:");
    // printMatrix(matrix1);
    printf("\nMatrix 1 generated!!!");
    int **matrix2 = randomMatrix();
    // printf("\nMatrix 2:");
    // printMatrix(matrix2);
    printf("\nMatrix 2 generated!!!");

    matrixSum = (int **)malloc(SIZE * sizeof(int *));
    for (i = 0; i < SIZE; i = i++)
    {
        matrixSum[i] = (int *)malloc(SIZE * sizeof(int));
        for (j = 0; j < SIZE; ++j)
        {
            matrixSum[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }

    printf("\nMatrix Sum:");
    // printMatrix(matrixSum);
    return 0;
}
