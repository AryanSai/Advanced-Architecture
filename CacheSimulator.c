#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define CACHE_SIZE_L1 16
#define CACHE_SIZE_L2 64
#define CACHE_LINE_SIZE 16
#define ASSOCIATIVITY 4

typedef struct CacheLine
{
    bool valid;                 // bool to keep track of validity of the line in the cache
    uint64_t tag;               
    char data[CACHE_LINE_SIZE]; // Data is stored as strings, hence each line is a character array
    int timestamp;              // For LRU replacement
} CacheLine;

typedef struct CacheSet
{
    CacheLine lines[ASSOCIATIVITY];
} CacheSet;

typedef struct CacheLevel1
{
    CacheSet sets[CACHE_SIZE_L1];
} CacheLevel1;

typedef struct CacheLevel2
{
    CacheLine lines[CACHE_SIZE_L2];
} CacheLevel2;

void initCacheLevel1(CacheLevel1 *cache)
{
    for (int i = 0; i < CACHE_SIZE_L1; i++)
    {
        for (int j = 0; j < ASSOCIATIVITY; j++)
        {
            cache->sets[i].lines[j].valid = false;
            cache->sets[i].lines[j].timestamp = 0;
        }
    }
}

void initCacheLevel2(CacheLevel2 *cacheL2)
{
    for (int i = 0; i < CACHE_SIZE_L2; i++)
    {
        cacheL2->lines[i].valid = false;
        cacheL2->lines[i].timestamp = 0;
    }
}

bool readCache(CacheLevel1 *cache, CacheLevel2 *cacheL2, uint64_t address, char *data)
{
    uint64_t tag = address / CACHE_LINE_SIZE; // Determine the tag
    int set_index = tag % CACHE_SIZE_L1;      // Determine the set index

    for (int i = 0; i < ASSOCIATIVITY; i++)
    {
        if (cache->sets[set_index].lines[i].valid && cache->sets[set_index].lines[i].tag == tag)
        {
            // Cache hit in L1 cache
            cache->sets[set_index].lines[i].timestamp++;
            *data = cache->sets[set_index].lines[i].data[address % CACHE_LINE_SIZE];
            return true;
        }
    }

    // Cache miss in L1 cache, check L2 cache
    for (int i = 0; i < CACHE_SIZE_L2; i++)
    {
        if (cacheL2->lines[i].valid && cacheL2->lines[i].tag == tag)
        {
            // Cache hit in L2 cache
            cacheL2->lines[i].timestamp++;
            *data = cacheL2->lines[i].data[address % CACHE_LINE_SIZE];

            // Copy data from L2 to L1 cache for future reference
            for (int j = 0; j < ASSOCIATIVITY; j++)
            {
                if (!cache->sets[set_index].lines[j].valid)
                {
                    cache->sets[set_index].lines[j] = cacheL2->lines[i];
                    break;
                }
            }
            // Cache miss in L1, hit in L2, so we count this as an L1 miss
            printf("L1 Cache Miss at address 0x%lx\n", address);
            return true;
        }
    }
    return false;
}

void writeCache(CacheLevel1 *cache, CacheLevel2 *cacheL2, uint64_t address, char data)
{
    uint64_t tag = address / CACHE_LINE_SIZE; // Determine the tag
    int set_index = tag % CACHE_SIZE_L1;      // Determine the set index

    // Find the least recently used cache line in the set
    int lruIndex = 0;
    for (int i = 0; i < ASSOCIATIVITY; i++)
    {
        if (!cache->sets[set_index].lines[i].valid || cache->sets[set_index].lines[i].timestamp < cache->sets[set_index].lines[lruIndex].timestamp)
        {
            lruIndex = i;
        }
    }

    // Update the least recently used cache line in L1 cache
    cache->sets[set_index].lines[lruIndex].valid = true;
    cache->sets[set_index].lines[lruIndex].tag = tag;
    cache->sets[set_index].lines[lruIndex].timestamp = 0; // Reset timestamp for LRU
    cache->sets[set_index].lines[lruIndex].data[address % CACHE_LINE_SIZE] = data;

    // Find the least recently used cache line in L2 cache
    lruIndex = 0;
    for (int i = 0; i < CACHE_SIZE_L2; i++)
    {
        if (!cacheL2->lines[i].valid || cacheL2->lines[i].timestamp < cacheL2->lines[lruIndex].timestamp)
        {
            lruIndex = i;
        }
    }

    // Update the least recently used cache line in L2 cache
    cacheL2->lines[lruIndex].valid = true;
    cacheL2->lines[lruIndex].tag = tag;
    cacheL2->lines[lruIndex].timestamp = 0; // Reset timestamp for LRU
    cacheL2->lines[lruIndex].data[address % CACHE_LINE_SIZE] = data;
}

int main()
{
    CacheLevel1 cache;
    CacheLevel2 cacheL2;

    // Initialise the cache
    initCacheLevel1(&cache);
    initCacheLevel2(&cacheL2);

    // Open the trace file to process
    FILE *traceFile = fopen("memory_trace.txt", "r");

    if (traceFile == NULL)
    {
        printf("Error: Unable to open memory trace file.\n");
        return 1;
    }

    char operation;   // The operation can be either a read(R) or a write(W)
    uint64_t address; // Address of the data location
    char data;        // The data to be cached in case of a Write operation

    // total reads; hits and misses count
    int total = 0, hits = 0, misses = 0;

    while (fscanf(traceFile, " %c %lx", &operation, &address) == 2)
    {
        if (operation == 'R')
        {
            total++;
            char readData; // the data read from the cache
            if (readCache(&cache, &cacheL2, address, &readData))
            {
                hits++;
                printf("Read from 0x%lx: Data = %c\n", (unsigned long)address, readData);
            }
            else
            {
                // Cache miss in both L1 and L2 caches
                printf("Cache Miss for address 0x%lx\n", (unsigned long)address);
                misses++;
            }
        }
        else if (operation == 'W')
        {
            if (fscanf(traceFile, " %c", &data) != 1)
            {
                printf("Error: Missing data in the Write Operation.\n");
                break;
            }
            writeCache(&cache, &cacheL2, address, data);
            printf("Write to 0x%lx: Data = %c\n", (unsigned long)address, data);
        }
        else
        {
            printf("Error: Invalid operation!\n");
            break;
        }
    }
    printf("\nTotal Number of Reads: %d\n", total);
    printf("Total Hits: %d\n", hits);
    printf("Total Misses: %d\n", misses);
    float ratio = (float)hits / total;
    printf("Hit Ratio: %f\n", ratio);
    fclose(traceFile);
    return 0;
}