#include <stdio.h>
#include <time.h>

void main()
{
    int n = 300000, a[n], b[n];
    clock_t start, end;
    for (int i = 0; i < n; i++)
    {
        a[i] = i;
    }
    // for (int i = 0; i < n; i++)
    // {
    //     b[i] = a[i];
    // }
    for (int i = 0; i < n; i++)
    {
        start = clock();
        b[i] = a[i];
        end = clock();
        printf("%f \n", (double)(end - start));
    }
    // printf("%f", end - start);
}