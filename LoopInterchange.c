// program to demonstrate the impact of loop interchange for cache optimization
#include <stdio.h>
#include <time.h>
void main()

{
	int i, j;
	int m[5000][100];
	int n[5000][100];
	clock_t start, stop;
	double d = 0.0;

	// storing data
	for (j = 0; j < 100; j = j + 1)
		for (i = 0; i < 5000; i = i + 1)
			m[i][j] = 1;

	for (i = 0; i < 5000; i = i + 1)
		for (j = 0; j < 100; j = j + 1)
			n[i][j] = 1;

	// experiment
	start = clock();
	for (j = 0; j < 100; j = j + 1)
		for (i = 0; i < 5000; i = i + 1)
			m[i][j] = 2 * m[i][j];

	stop = clock();
	d = (double)(stop - start) / CLOCKS_PER_SEC;
	printf("The run-time of row major order is %lf\n", d);

	start = clock();
	for (i = 0; i < 5000; i = i + 1)
		for (j = 0; j < 100; j = j + 1)
			n[i][j] = 2 * n[i][j];

	stop = clock();
	d = (double)(stop - start) / CLOCKS_PER_SEC;
	printf("The run-time of column major order is %lf\n", d);
}

// This code is helpful in finding out the impact of interchanging the loops on the performance of accessing a two-dimensional array. The code is designed to compare the runtime of two different loop ordering schemes: row-major order and column-major order.

// In programming languages like C, memory layout affects the efficiency of accessing arrays, especially multi-dimensional ones. In a row-major order, elements in the same row are stored together in memory, while in a column-major order, elements in the same column are stored together.

// By interchanging the loops, you are essentially switching between these two orders and observing the impact on runtime. Row-major order is typically more efficient when accessing consecutive elements within a row, while column-major order is more efficient when accessing consecutive elements within a column.

// Here's what the code does:

// 1. It initializes two 2D arrays `m` and `n` with the same values.
// 2. It then performs the same computation on both arrays using two different loop orders: one using a row-major order and the other using a column-major order.
// 3. The code measures the time taken to perform these computations using the `clock()` function and calculates the difference in time.
// 4. Finally, it prints the runtimes of both loop orders.

// By comparing the runtimes of the two loop orders, you can observe which one is more efficient for this specific computation on your system. The difference in runtime will give you insights into the impact of loop ordering on memory access patterns and cache efficiency.


// The output you've provided indicates that the column-major order is more efficient than the row-major order for the specific computation you're performing in your code.

// In this case, the column-major order has a shorter runtime (0.001470 seconds) compared to the row-major order (0.003190 seconds). This suggests that the memory access patterns of the column-major order are better suited to the cache behavior of your system, resulting in faster access times.

// Keep in mind that the specific behavior may vary based on factors such as the hardware architecture, cache sizes, and the exact computation being performed. The performance difference you observed here might not hold true for all scenarios, so it's important to consider the specific context in which you're working.

// If you're dealing with larger data sets or different types of computations, the performance difference between row-major and column-major orders might vary. It's always a good idea to profile and test your code with realistic scenarios to determine the most optimal loop ordering for your specific use case.