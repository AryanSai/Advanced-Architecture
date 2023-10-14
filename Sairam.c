#define __STDC_FORMAT_MACROS
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define HISTORY_TABLE_SIZE 256

int total = 0;

typedef struct
{
    char prediction_bit;
    char hysteresis_bit;
} LocalHistoryEntry;

LocalHistoryEntry localHistoryTable[HISTORY_TABLE_SIZE];

char *intToBinaryString(int num)
{
    int bitCount = sizeof(num) * 8;
    char *binaryString = (char *)malloc(bitCount + 1);
    if (binaryString == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    int startIndex = 0;
    for (int i = 0; i < bitCount; i++)
    {
        int bit = (num >> (bitCount - 1 - i)) & 1;
        if (bit == 1 || startIndex > 0)
        {
            binaryString[startIndex++] = bit + '0';
        }
    }

    if (startIndex == 0)
    {
        binaryString[startIndex++] = '0';
    }
    binaryString[startIndex] = '\0';
    return binaryString;
}

void localHistoryPredictor(uint64_t instructionAddress, char str[])
{
    uint16_t index = (uint16_t)(instructionAddress & 0xFF);

    // Get the local history entry for the current instruction
    LocalHistoryEntry *entry = &localHistoryTable[index];

    printf("%s,%s", entry->hysteresis_bit, entry->prediction_bit);
    // Implement your local history predictor logic here
    // You can use entry->prediction_bit and entry->hysteresis_bit to make predictions
    // Update the entry based on the actual outcome (str) after making a prediction
}

void simulate(FILE *inputFile, FILE *outputFile)
{
    // See the documentation to understand what these variables mean.
    int32_t microOpCount;
    uint64_t instructionAddress;
    int32_t sourceRegister1;
    int32_t sourceRegister2;
    int32_t destinationRegister;
    char conditionRegister;
    char TNnotBranch;
    char loadStore;
    int64_t immediate;
    uint64_t addressForMemoryOp;
    uint64_t fallthroughPC;
    uint64_t targetAddressTakenBranch;
    char macroOperation[12];
    char microOperation[23];

    int64_t totalMicroops = 0;
    int64_t totalMacroops = 0;

    fprintf(outputFile, "Processing trace...\n");

    // table
    char table[256][2];
    for (int i = 0; i < 256; i++)
    {
        table[i][0] = '1';
        table[i][1] = '1';
    }

    while (true)
    {
        int result = fscanf(inputFile,
                            "%" SCNi32
                            "%" SCNx64
                            "%" SCNi32
                            "%" SCNi32
                            "%" SCNi32
                            " %c"
                            " %c"
                            " %c"
                            "%" SCNi64
                            "%" SCNx64
                            "%" SCNx64
                            "%" SCNx64
                            "%11s"
                            "%22s",
                            &microOpCount,
                            &instructionAddress,
                            &sourceRegister1,
                            &sourceRegister2,
                            &destinationRegister,
                            &conditionRegister,
                            &TNnotBranch,
                            &loadStore,
                            &immediate,
                            &addressForMemoryOp,
                            &fallthroughPC,
                            &targetAddressTakenBranch,
                            macroOperation,
                            microOperation);

        if (result == EOF)
        {
            break;
        }
        if (result != 14)
        {
            fprintf(stderr, "Error parsing trace at line %" PRIi64 "\n", totalMicroops);
            abort();
        }

        if (targetAddressTakenBranch != 0 && conditionRegister == 'R')
        {
            uint16_t extracted_bits = (uint16_t)(instructionAddress & 0xFF);
            int index = (int)extracted_bits;

            char bit_string[3];
            strcpy(bit_string, table[index]);
            localHistoryPredictor(instructionAddress, bit_string);
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *inputFile = stdin;
    FILE *outputFile = stdout;

    if (argc >= 2)
    {
        inputFile = fopen(argv[1], "r");
        assert(inputFile != NULL);
    }
    if (argc >= 3)
    {
        outputFile = fopen(argv[2], "w");
        assert(outputFile != NULL);
    }
    // Initialize the local history table
    for (int i = 0; i < HISTORY_TABLE_SIZE; i++)
    {
        localHistoryTable[i].prediction_bit = '1';
        localHistoryTable[i].hysteresis_bit = '1';
    }

    // Call the simulation function
    simulate(inputFile, outputFile);

    // Close input and output files if necessary

    return 0;
}
