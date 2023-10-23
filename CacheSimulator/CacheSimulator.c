#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define L1_CACHE_SIZE (128 * 1024)  // 128 KiB = 131.072 KB
#define L2_CACHE_SIZE (1024 * 1024) // 1 MiB = 2^20 bytes = 1.048576 MB
#define ASSOCIATIVITY 4
#define LINE_SIZE 32 // 32 bytes
#define L1_CACHE_SETS (L1_CACHE_SIZE / (LINE_SIZE * ASSOCIATIVITY))
#define L2_CACHE_LINES (L2_CACHE_SIZE / LINE_SIZE)

int missPenalty = 0; // Integer variable to calculate miss penalty

typedef struct CacheLine
{
    bool valid;           // Boolean to keep track of validity of the line in the cache
    uint64_t tag;         // tag value
    char data[LINE_SIZE]; // Data is stored as strings, hence each line is a character array
    int timestamp;        // For LRU replacement
} CacheLine;

typedef struct CacheSet
{
    CacheLine lines[ASSOCIATIVITY];
} CacheSet;

typedef struct CacheLevel1 // L1 Cache is 4-Way Set Assosciative
{
    CacheSet sets[L1_CACHE_SETS];
} CacheLevel1;

typedef struct CacheLevel2 // L2 Cache is direct-mapped
{
    CacheLine lines[L2_CACHE_LINES];
} CacheLevel2;

// Initialising L1 cache
void initCacheLevel1(CacheLevel1 *cache)
{
    for (int i = 0; i < L1_CACHE_SETS; i++)
    {
        for (int j = 0; j < ASSOCIATIVITY; j++)
        {
            cache->sets[i].lines[j].valid = false;
            cache->sets[i].lines[j].timestamp = 0;
        }
    }
}

// Initialising L2 cache
void initCacheLevel2(CacheLevel2 *cacheL2)
{
    for (int i = 0; i < L2_CACHE_LINES; i++)
    {
        cacheL2->lines[i].valid = false;
        cacheL2->lines[i].timestamp = 0;
    }
}

// Function to read caches
bool readCache(CacheLevel1 *cache, CacheLevel2 *cacheL2, uint64_t address, char *data)
{
    int line_address = address / LINE_SIZE;       // ByteAddress/LineSize
    int set_index = line_address % L1_CACHE_SETS; // Determine the set index
    int tag = line_address / L1_CACHE_SETS;       // Determine the tag

    // Check L1 cache
    for (int i = 0; i < ASSOCIATIVITY; i++)
    { // Checking for validity and tag match
        if (cache->sets[set_index].lines[i].valid && cache->sets[set_index].lines[i].tag == tag)
        {
            // Cache hit in L1 cache
            cache->sets[set_index].lines[i].timestamp++;
            *data = cache->sets[set_index].lines[i].data[line_address];
            return true; // Returning Positive
        }
    }

    // Cache miss in L1 cache, check L2 cache
    for (int i = 0; i < L2_CACHE_LINES; i++)
    { // Checking for validity and tag match
        if (cacheL2->lines[i].valid && cacheL2->lines[i].tag == tag)
        {
            // Cache hit in L2 cache
            cacheL2->lines[i].timestamp++;
            *data = cacheL2->lines[i].data[line_address];

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
            missPenalty = missPenalty + 5; // Miss penaly of L1 Cache
            printf("L1 Cache Miss at address 0x%lx\n", address);
            return true; // Returning Positive
        }
        else
        {
            missPenalty = missPenalty + 10; // Miss penaly of L2 Cache
            return false;                   // Returning Negative
        }
    }
}

void writeCache(CacheLevel1 *cache, CacheLevel2 *cacheL2, uint64_t address, char data)
{
    int line_address = address / LINE_SIZE;
    int set_index = line_address % L1_CACHE_SETS; // Determine the set index
    int tag = line_address / L1_CACHE_SETS;       // Determine the tag

    // Find the least recently used cache line in the set for replacement
    int lruIndex = 0; // Variable to find the LRU index
    for (int i = 0; i < ASSOCIATIVITY; i++)
    { // checking for invalidity and timestamp
        if (!cache->sets[set_index].lines[i].valid || cache->sets[set_index].lines[i].timestamp < cache->sets[set_index].lines[lruIndex].timestamp)
        {
            lruIndex = i;
        }
    }

    // Update the least recently used cache line in L1 cache
    cache->sets[set_index].lines[lruIndex].valid = true;
    cache->sets[set_index].lines[lruIndex].tag = tag;
    cache->sets[set_index].lines[lruIndex].timestamp = 0; // Reset timestamp for LRU
    cache->sets[set_index].lines[lruIndex].data[line_address] = data;

    // Find the least recently used cache line in L2 cache
    lruIndex = 0;
    for (int i = 0; i < L2_CACHE_LINES; i++)
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
    cacheL2->lines[lruIndex].data[line_address] = data;
}

int main()
{
    CacheLevel1 cache;
    CacheLevel2 cacheL2;

    // Initialise the cache
    initCacheLevel1(&cache);
    initCacheLevel2(&cacheL2);

    // Open the trace file to process
    FILE *traceFile = fopen("1600.trace", "r");

    // Error Handling for incorrect trace file
    if (traceFile == NULL)
    {
        printf("Error: Unable to open memory trace file.\n");
        return 1;
    }

    char operation;   // The operation can be either a read(R) or a write(W)
    uint64_t address; // Address of the data location
    char data;        // The data to be cached in case of a Write operation

    // Total number of reads; Total number of hits and misses count
    int count = 0, reads = 0, hits = 0, misses = 0;

    // Processing the trace file
    while (fscanf(traceFile, " %c %lx", &operation, &address) == 2)
    {
        count++; // Counting the number instructions
        if (operation == 'R')
        {
            reads++;       // Incrementing the number of reads
            char readData; // the data read from the cache
            if (readCache(&cache, &cacheL2, address, &readData))
            {
                hits++; // Incrementing the number of hits
                printf("Read from 0x%x: Data = %c\n", (unsigned)address, readData);
            }
            else
            {
                // Cache miss in both L1 and L2 caches
                printf("Cache Miss for address 0x%x\n", (unsigned)address);
                misses++; // Incrementing the number of misses
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
            printf("Write to 0x%x: Data = %c\n", (unsigned)address, data);
        }
        else
        {
            printf("Error: Invalid operation!\n");
            break;
        }
    }
    // Printing the results
    printf("\nTotal Number of Instructions Read: %d\n", count);

    printf("\nTotal Number of Reads: %d\n", reads);

    printf("\nTotal Hits: %d\n", hits);
    float hitRatio = (float)hits / reads;
    printf("\nHit Ratio: %f\n", hitRatio);

    printf("\nTotal Misses: %d\n", misses);
    float missRatio = (float)misses / reads;
    printf("\nMiss Ratio: %f\n", missRatio);

    printf("\nTotal Miss Penalty: %d\n", missPenalty);
    fclose(traceFile);
    return 0;
}