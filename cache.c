#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define CACHE_SIZE 16
#define CACHE_LINE_SIZE 64

typedef struct CacheLine
{
    uint64_t tag;
    char data[CACHE_LINE_SIZE];
    bool valid;
    uint64_t timestamp; // Used for LRU replacement
} CacheLine;

typedef struct Cache
{
    CacheLine lines[CACHE_SIZE];
} Cache;

void initCache(Cache *cache)
{
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        cache->lines[i].valid = false;
        cache->lines[i].timestamp = 0;
    }
}

bool readCache(Cache *cache, uint64_t address, char *data)
{
    uint64_t tag = address / CACHE_LINE_SIZE;

    for (int i = 0; i < CACHE_SIZE; i++)
    {
        if (cache->lines[i].valid && cache->lines[i].tag == tag)
        {
            // Cache hit
            cache->lines[i].timestamp++; // Update timestamp for LRU
            *data = cache->lines[i].data[address % CACHE_LINE_SIZE];
            return true;
        }
    }

    // Cache miss
    return false;
}

void writeCache(Cache *cache, uint64_t address, char data)
{
    uint64_t tag = address / CACHE_LINE_SIZE;

    // Find the least recently used cache line
    int lruIndex = 0;
    for (int i = 1; i < CACHE_SIZE; i++)
    {
        if (!cache->lines[i].valid || cache->lines[i].timestamp < cache->lines[lruIndex].timestamp)
        {
            lruIndex = i;
        }
    }

    // Update the least recently used cache line
    cache->lines[lruIndex].valid = true;
    cache->lines[lruIndex].tag = tag;
    cache->lines[lruIndex].timestamp = 0; // Reset timestamp for LRU
    cache->lines[lruIndex].data[address % CACHE_LINE_SIZE] = data;
}

int main()
{
    Cache cache;
    initCache(&cache);

    // Example addresses and data
    uint64_t addresses[] = {0x1234, 0x5678, 0x9ABC};
    char data[] = {'A', 'B', 'C'};

    // Write data to the cache
    for (int i = 0; i < sizeof(addresses) / sizeof(addresses[0]); i++)
    {
        uint64_t address = addresses[i];
        char value = data[i];

        printf("Writing data to address 0x%lx: %c\n", address, value);
        writeCache(&cache, address, value);
    }

    // Read data from the cache
    for (int i = 0; i < sizeof(addresses) / sizeof(addresses[0]); i++)
    {
        uint64_t address = addresses[i];
        char readData;

        if (readCache(&cache, address, &readData))
        {
            printf("Cache hit for address 0x%lx: Data = %c\n", address, readData);
        }
        else
        {
            printf("Cache miss for address 0x%lx\n", address);
        }
    }

    // Address and data for cache miss
    uint64_t missAddress = 0xDEAD;
    char missData = 'D';

    // Attempt to read data from the cache
    char readData;

    if (readCache(&cache, missAddress, &readData))
    {
        printf("Cache hit for address 0x%lx: Data = %c\n", missAddress, readData);
    }
    else
    {
        printf("Cache miss for address 0x%lx\n", missAddress);
    }

    return 0;
}