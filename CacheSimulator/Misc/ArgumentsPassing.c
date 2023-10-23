#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

int missPenalty = 0;
int l1_cache_size = 0, l2_cache_size = 0, line_size = 0, associativity = 0, l1_cache_sets = 0, l2_cache_lines = 0;

typedef struct CacheLine
{
    bool valid; // Boolean to keep track of the validity of the line in the cache
    uint64_t tag;
    char *data;    // Data is stored as strings, allocated dynamically
    int timestamp; // For LRU replacement
} CacheLine;

typedef struct CacheSet
{
    CacheLine *lines; // Array of CacheLine
} CacheSet;

typedef struct CacheLevel1 // L1 Cache is 4-Way Set Associative
{
    CacheSet *sets; // Array of CacheSet
} CacheLevel1;

typedef struct CacheLevel2 // L2 Cache is direct-mapped
{
    CacheLine *lines; // Array of CacheLine
} CacheLevel2;

void initCacheLevel1(CacheLevel1 *cache)
{
    for (int i = 0; i < l1_cache_sets; i++)
    {
        cache->sets[i].lines = malloc(associativity * sizeof(CacheLine)); // Allocate memory for lines within each set
        if (cache->sets[i].lines == NULL)
        {
            // Handle allocation failure (out of memory)
            // You might want to add error handling code here.
        }
        for (int j = 0; j < associativity; j++)
        {
            cache->sets[i].lines[j].valid = false;
            cache->sets[i].lines[j].timestamp = 0;
            cache->sets[i].lines[j].data = malloc(line_size * sizeof(char));
            if (cache->sets[i].lines[j].data == NULL)
            {
                // Handle allocation failure (out of memory)
                // You might want to add error handling code here.
            }
        }
    }
}

void initCacheLevel2(CacheLevel2 *cacheL2)
{
    cacheL2->lines = malloc(l2_cache_lines * sizeof(CacheLine));
    if (cacheL2->lines == NULL)
    {
        // Handle allocation failure (out of memory)
        // You might want to add error handling code here.
    }
    for (int i = 0; i < l2_cache_lines; i++)
    {
        cacheL2->lines[i].valid = false;
        cacheL2->lines[i].timestamp = 0;
        cacheL2->lines[i].data = malloc(line_size * sizeof(char));
    }
}

bool readCache(CacheLevel1 *cache, CacheLevel2 *cacheL2, uint64_t address, char *data)
{
    int line_address = address / line_size;       // ByteAddress/LineSize
    int set_index = line_address % l1_cache_sets; // Determine the set index
    int tag = line_address / l1_cache_sets;       // Determine the tag

    for (int i = 0; i < associativity; i++)
    {
        if (cache->sets[set_index].lines[i].valid && cache->sets[set_index].lines[i].tag == tag)
        {
            // Cache hit in L1 cache
            cache->sets[set_index].lines[i].timestamp++;
            *data = cache->sets[set_index].lines[i].data[line_address];
            return true;
        }
    }

    // Cache miss in L1 cache, check L2 cache
    for (int i = 0; i < l2_cache_lines; i++)
    {
        if (cacheL2->lines[i].valid && cacheL2->lines[i].tag == tag)
        {
            // Cache hit in L2 cache
            cacheL2->lines[i].timestamp++;
            *data = cacheL2->lines[i].data[line_address];

            // Copy data from L2 to L1 cache for future reference
            for (int j = 0; j < associativity; j++)
            {
                if (!cache->sets[set_index].lines[j].valid)
                {
                    cache->sets[set_index].lines[j] = cacheL2->lines[i];
                    break;
                }
            }

            // Cache miss in L1, hit in L2, so we count this as an L1 miss
            missPenalty = missPenalty + 5; // Miss penalty of L1 Cache
            printf("L1 Cache Miss at address 0x%lx\n", address);
            return true;
        }
    }

    missPenalty = missPenalty + 10; // Miss penalty of L2 Cache
    return false;
}

void writeCache(CacheLevel1 *cache, CacheLevel2 *cacheL2, uint64_t address, char data)
{
    int line_address = address / line_size;
    int set_index = line_address % l1_cache_sets; // Determine the set index
    int tag = line_address / l1_cache_sets;       // Determine the tag

    // Find the least recently used cache line in the set
    int lruIndex = 0;
    for (int i = 0; i < associativity; i++)
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
    cache->sets[set_index].lines[lruIndex].data[line_address] = data;

    // Find the least recently used cache line in L2 cache
    lruIndex = 0;
    for (int i = 0; i < l2_cache_lines; i++)
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

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Usage: %s L1_CACHE_SIZE L2_CACHE_SIZE LINE_SIZE ASSOCIATIVITY\n", argv[0]);
        return 1;
    }

    l1_cache_size = atoi(argv[1]);
    l2_cache_size = atoi(argv[2]);
    line_size = atoi(argv[3]);
    associativity = atoi(argv[4]);

    l1_cache_size = l1_cache_size * 1024;
    l2_cache_size = l2_cache_size * 1024;
    l1_cache_sets = l1_cache_size / (line_size * associativity);
    l2_cache_lines = l2_cache_size / line_size;

    // Create an array of CacheSet and CacheLine for L1 and L2 caches
    CacheLevel1 cache;
    cache.sets = malloc(l1_cache_sets * sizeof(CacheSet));
    for (int i = 0; i < l1_cache_sets; i++)
    {
        cache.sets[i].lines = malloc(associativity * sizeof(CacheLine));
    }

    CacheLevel2 cacheL2;
    cacheL2.lines = malloc(l2_cache_lines * sizeof(CacheLine));

    // Initialize the cache
    initCacheLevel1(&cache);
    initCacheLevel2(&cacheL2);

    FILE *traceFile = fopen("12K.trace", "r");

    if (traceFile == NULL)
    {
        printf("Error: Unable to open memory trace file.\n");
        return 1;
    }

    char operation;
    uint64_t address;
    char data;
    int count = 0, total = 0, hits = 0, misses = 0;

    while (fscanf(traceFile, " %c %lx", &operation, &address) == 2)
    {
        count++;
        if (operation == 'R')
        {
            total++;
            char readData;
            if (readCache(&cache, &cacheL2, address, &readData))
            {
                hits++;
                printf("Read from 0x%lx: Data = %c\n", (unsigned long)address, readData);
            }
            else
            {
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

    if (ferror(traceFile))
    {
        printf("Error reading the memory trace file.\n");
    }

    printf("\nTotal Number of Instructions Read: %d\n", count);
    printf("\nTotal Number of Reads: %d\n", total);
    printf("\nTotal Hits: %d\n", hits);
    printf("\nTotal Misses: %d\n", misses);
    float hitRatio = (float)hits / total;
    printf("\nHit Ratio: %f\n", hitRatio);
    float missRatio = (float)misses / total;
    printf("\nMiss Ratio: %f\n", missRatio);
    printf("\nTotal Miss Penalty: %d\n", missPenalty);

    fclose(traceFile);

    // Free dynamically allocated memory when done
    for (int i = 0; i < l1_cache_sets; i++)
    {
        for (int j = 0; j < associativity; j++)
        {
            free(cache.sets[i].lines[j].data);
        }
        free(cache.sets[i].lines);
    }
    free(cache.sets);

    for (int i = 0; i < l2_cache_lines; i++)
    {
        free(cacheL2.lines[i].data);
    }
    free(cacheL2.lines);

    return 0;
}