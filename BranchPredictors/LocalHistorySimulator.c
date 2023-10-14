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
int total = 0;

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

void twoBitPredictor(char str[], char bits[])
{
  char prediction_bit = bits[0];
  char hysteresis_bit = bits[1];
  int predictions = 0, mispredictions = 0;
  float mispredictionRate = 0.0;

  for (int i = 0; i < total; i++)
  {
    char bit = str[i];
    if (bit == '1')
    {
      if (prediction_bit == '1' && hysteresis_bit == '1')
      {
        predictions++;
      }
      else if (prediction_bit == '1' && hysteresis_bit == '0')
      {
        hysteresis_bit = '1';
        predictions++;
      }
      else if (prediction_bit == '0' && hysteresis_bit == '1')
      {
        prediction_bit = '1';
        hysteresis_bit = '0';
        mispredictions++;
      }
      else
      {
        hysteresis_bit = '1';
        mispredictions++;
      }
    }
    else
    {
      if (prediction_bit == '1' && hysteresis_bit == '1')
      {
        hysteresis_bit = '0';
        mispredictions++;
      }
      else if (prediction_bit == '1' && hysteresis_bit == '0')
      {
        prediction_bit = '0';
        hysteresis_bit = '1';
        mispredictions++;
      }
      else if (prediction_bit == '0' && hysteresis_bit == '1')
      {
        hysteresis_bit = '0';
        predictions++;
      }
      else
      {
        predictions++;
      }
    }
  }
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

      char bit_string[3]; // Make sure there is enough space for the null terminator
      strcpy(bit_string, table[index]);

      twoBitPredictor(bit_string, table[index]);
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

  simulate(inputFile, outputFile);
  return 0;
}
