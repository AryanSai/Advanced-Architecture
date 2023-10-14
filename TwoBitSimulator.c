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

#define SIZE 5
int total = 0;

void twoBitPredictor(char str[])
{
  char prediction_bit = '1';
  char hysteresis_bit = '1';
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
  mispredictionRate = (float)mispredictions / (mispredictions + predictions);

  printf("Total Predictions = %d \n", predictions);
  printf("Total MisPredictions = %d \n", mispredictions);
  printf("MisPrediction Rate = %f \n", mispredictionRate);
  printf("Accuracy Rate = %f \n", 1 - mispredictionRate);
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

  char bitString[1000];
  bitString[0] = '\0';
  char t = '1';
  char nt = '0';
  int branchCount = 0;

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

    if (targetAddressTakenBranch != 0 && conditionRegister == 'R')
    {
      if (TNnotBranch == 'T')
      {
        strncat(bitString, &t, 1);
        total++;
      }
      else if (TNnotBranch == 'N')
      {
        strncat(bitString, &nt, 1);
        total++;
      }
      branchCount++;
    }

    if (result != 14)
    {
      fprintf(stderr, "Error parsing trace at line %" PRIi64 "\n", totalMicroops);
      abort();
    }
  }
  fprintf(outputFile, "%s", bitString);
  twoBitPredictor(bitString);
  // printf("%s", bitString);
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
