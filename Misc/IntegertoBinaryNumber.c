#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> // For strcpy

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

int main()
{
    uint64_t value = 0x48D1DE;
    uint16_t extracted_value = (uint16_t)(value & 0xFF);
    printf("Extracted value: %X\n", extracted_value);
    printf("%d\n", extracted_value);
    int decimal = (int)extracted_value;

    char sai[50];
    char *binaryString = intToBinaryString(decimal);
    strcpy(sai, binaryString);
    free(binaryString);

    printf("Binary representation: %s\n", sai);

    return 0;
}
