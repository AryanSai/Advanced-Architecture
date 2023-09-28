#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100
#define B 32 // Blocking factor

void BeforeBlocking(int x[N][N], int y[N][N], int z[N][N])
{
    int i, j, k;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            int r = 0;
            for (k = 0; k < N; k++)
            {
                r = r + y[i][k] * z[k][j];
            }
            x[i][j] = r;
        }
    }
}

void AfterBlocking(int x[N][N], int y[N][N], int z[N][N])
{
    int i, j, k, jj, kk;
    for (jj = 0; jj < N; jj += B)
    {
        for (kk = 0; kk < N; kk += B)
        {
            for (i = 0; i < N; i++)
            {
                for (j = jj; j < jj + B; j++)
                {
                    int r = 0;
                    for (k = kk; k < kk + B; k++)
                    {
                        r = r + y[i][k] * z[k][j];
                    }
                    x[i][j] = x[i][j] + r;
                }
            }
        }
    }
}

int main()
{
    int(*x)[N] = malloc(N * sizeof(*x));
    int(*y)[N] = malloc(N * sizeof(*y));
    int(*z)[N] = malloc(N * sizeof(*z));

    if (x == NULL || y == NULL || z == NULL)
    {
        perror("Memory allocation failed");
        return 1;
    }

    int i, j;

    // Initialize matrices y and z with random values
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            y[i][j] = rand() % 100;
            z[i][j] = rand() % 100;
        }
    }

    // Measure time for matrix multiplication before blocking
    clock_t start = clock();
    BeforeBlocking(x, y, z);
    clock_t stop = clock();
    double timeBeforeBlocking = (double)(stop - start) / CLOCKS_PER_SEC;

    // Measure time for matrix multiplication after blocking
    start = clock();
    AfterBlocking(x, y, z);
    stop = clock();
    double timeAfterBlocking = (double)(stop - start) / CLOCKS_PER_SEC;

    printf("Time before blocking: %lf seconds\n", timeBeforeBlocking);
    printf("Time after blocking: %lf seconds\n", timeAfterBlocking);
    return 0;
}

// Blocking is an optimization technique used to improve memory access patterns and cache efficiency, particularly in cases where multiple arrays are accessed with orthogonal access patterns (both by rows and columns). Blocking involves dividing the data into smaller submatrices or blocks and performing computations on these blocks to enhance spatial and temporal locality.
