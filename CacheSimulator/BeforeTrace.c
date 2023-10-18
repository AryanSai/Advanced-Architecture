/*  Mini Project : MTCS-102 P

    Submitted by: Aryan Sai Arvapelly, Reg. No. 23352, 1st MTech CS

    Goal: Simulating a 2 Level Cache where L1 cache is 4-Way Set Assosciative and L2 is direct-mapped.
          The replacement policy used is Least Recently Used(LRU).
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define CACHE_SIZE_L1 16
#define CACHE_SIZE_L2 64
#define CACHE_LINE_SIZE 16
#define ASSOCIATIVITY 4

typedef struct CacheLine
{
    bool valid;
    uint64_t tag;
    char data[CACHE_LINE_SIZE];
    int timestamp; // For LRU replacement
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
    int set_index = tag % CACHE_SIZE_L1; // Determine the set index

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
            // printf("L1 Cache Miss at address 0x%lx, Fetching from L2\n", address);

            return true;
        }
    }

    // Cache miss in L2 cache as well, count as L1 and L2 miss
    printf("L1 Cache Miss at address 0x%lx, Fetching from L2\n", address);

    return false;
}

void writeCache(CacheLevel1 *cache, CacheLevel2 *cacheL2, uint64_t address, char data)
{
    uint64_t tag = address / CACHE_LINE_SIZE;
    int set_index = tag % CACHE_SIZE_L1; // Determine the set index

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

    initCacheLevel1(&cache);
    initCacheLevel2(&cacheL2);

    // Example addresses and data
    uint64_t addresses[] = {0x1234, 0x5678, 0x9ABC, 0xDEAD, 0xBEEF, 0xC0FF};
    char data[] = {'A', 'B', 'C', 'D', 'E', 'F'};

    // Write data to the cache
    for (int i = 0; i < sizeof(addresses) / sizeof(addresses[0]); i++)
    {
        uint64_t address = addresses[i];
        char value = data[i];

        printf("Writing data to address 0x%lx: %c\n", address, value);
        writeCache(&cache, &cacheL2, address, value);
    }

    // Read data from the cache
    for (int i = 0; i < sizeof(addresses) / sizeof(addresses[0]); i++)
    {
        uint64_t address = addresses[i];
        char readData;

        if (readCache(&cache, &cacheL2, address, &readData))
        {
            printf("L1 Cache Hit for address 0x%lx: Data = %c\n", address, readData);
        }
        else
        {
            printf("L1 Cache Miss for address 0x%lx\n", address);
        }
    }

    return 0;
}